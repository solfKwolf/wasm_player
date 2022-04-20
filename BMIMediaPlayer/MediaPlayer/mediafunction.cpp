#include "mediafunction.h"
#include "ffmediafactory.h"
#include <packet.h>
#include <frame.h>
#include <demux.h>
#include <stream.h>
#include <decode.h>
#include <scale.h>
#include "mediadata.h"
#include "playentrancethread.h"
#include "renderparamcontrol.h"
#include "mediacallback.h"
#include <iostream>

MediaFunction::MediaFunction()
    : m_pMediaFac(new FFMediaFactory)
    , m_pDemuxerUnknow(nullptr)
    , m_pDemuxThread(NULL)
    , m_pCallBack(new MediaCallBack)
    , m_audioRenderThread(NULL)
    , m_videoRenderThread(NULL)
    , m_pVideoScaleUNknown(NULL)
    , m_pAudioScaleUNknown(NULL)

{
    auto callback_playInThread = std::bind(&MediaFunction::playInThread,this);
    m_pEntrance = new PlayEntranceThread(callback_playInThread);
}

MediaFunction::~MediaFunction()
{
    if(m_pMediaFac)
    {
        delete m_pMediaFac;
        m_pMediaFac = nullptr;
    }
    if(m_pEntrance)
    {
        m_pEntrance->wait();
        delete m_pEntrance;
        m_pEntrance = nullptr;
    }
    if(m_pDemuxerUnknow)
    {
        m_pDemuxerUnknow->Release();
        m_pDemuxerUnknow = nullptr;
    }
    if(m_pAudioScaleUNknown)
    {
        m_pAudioScaleUNknown->Release();
        m_pAudioScaleUNknown = nullptr;
    }
    if(m_pVideoScaleUNknown)
    {
        m_pVideoScaleUNknown->Release();
        m_pVideoScaleUNknown = nullptr;
    }
}

void MediaFunction::playInThread()
{
    m_pDemuxerUnknow = m_pMediaFac->createDemuxerUNknown();

    if(!m_pDemuxerUnknow)
    {
        std::cout << "load ffmpeg demuxer error." << std::endl;
        return;
    }

    IPtr<IDemux, &IID_IDEMUX> spIDemuxer(m_pDemuxerUnknow);

    IDemux::IDemuxError ret = IDemux::NoError;
    if(!MediaData::getInstance()->getIODevice())
    {
        ret = spIDemuxer->initDemuxer(MediaData::getInstance()->getFilename());
    }
    else
    {
        auto callback_ioStream = std::bind(&MediaFunction::readData,this,std::placeholders::_1,std::placeholders::_2);
        auto callback_seek = std::bind(&MediaFunction::seekData,this,std::placeholders::_1,std::placeholders::_2);
        ret = spIDemuxer->initDemuxer(callback_ioStream,callback_seek);
    }
    if(ret != IDemux::NoError)
    {
        std::cout << "init demuxer error." << std::endl;
        if(m_pCallBack)
        {
            m_pCallBack->openFinshType(0);
        }
        return;
    }
    int nStreamCount = spIDemuxer->getStreamCount();
    MediaData::getInstance()->setStreamCount(nStreamCount);

    std::cout << "stream count is: " << nStreamCount<<std::endl;

    for(int i = 0; i < nStreamCount; i++)
    {
        IUNknown* pUNknow = m_pMediaFac->createStreamUNknown();
        spIDemuxer->getStream(i, pUNknow);
        IPtr<IAVStream, &IID_ISTREAM> spIStream(pUNknow);
        if(spIStream)
        {
            if(spIStream->getStreamType() == IAVStream::AVMediaType::AVTypeVideo)
            {
                int nVideoWidth = -1;
                int nVideoHeight = -1;
                int nPixelFormat = IFrame::FrameFormat::FormatRGBA;
                spIStream->getFrameSize(nVideoWidth,nVideoHeight);
                m_pCallBack->setVideoFormat(nVideoWidth,nVideoHeight,nPixelFormat);
                MediaData::getInstance()->setVideoFormat(nVideoWidth,nVideoHeight,nPixelFormat);
                MediaData::getInstance()->setVStreamIndex(i);
            }
            else if(spIStream->getStreamType() == IAVStream::AVMediaType::AVTypeAudio)
            {
                int samepleRate = -1;
                int channelCount = -1;
                IAVStream::AudioSampleFormat sampleFormat = IAVStream::AudioFormateNone;
                spIStream->getAudioFormat(samepleRate,channelCount,sampleFormat);

                int nSampleFormat = sampleFormat;
                m_pCallBack->setAudioFormat(samepleRate,channelCount, nSampleFormat);
                sampleFormat = (IAVStream::AudioSampleFormat)nSampleFormat;
                MediaData::getInstance()->setAudioFormat(samepleRate,channelCount,sampleFormat);
                MediaData::getInstance()->setAStreamIndex(i);
            }
            else if(spIStream->getStreamType() == IAVStream::AVMediaType::AVTypeSubtitle)
            {
                MediaData::getInstance()->setSStreamIndex(i);
            }
        }
        IUNknown* pDecoderUnknow = m_pMediaFac->createDecoderUNknown();
        IPtr<IDecode, &IID_IDECODE> spIDecoder(pDecoderUnknow);
        spIDecoder->setHardware(MediaData::getInstance()->getHardware());
        IDecode::IDecodeError ret = spIDecoder->initDecoder(pUNknow);
        if(ret == IDecode::CodecOpenError)
        {
            pUNknow->Release();
            pDecoderUnknow->Release();
        }
        else
        {
            m_streamVec.push_back(pUNknow);
            m_decoderVec.push_back(pDecoderUnknow);
        }
    }
    if(m_pCallBack)
    {
        m_pCallBack->openFinshType(1);
    }
    openFileSuccess();
}

void MediaFunction::openFileSuccess()
{
    initIUNknownQueue();
    //创建解复用线程
    createDemuxThread(m_pDemuxerUnknow);

    //创建解码用线程
    for(int i = 0; i< MediaData::getInstance()->getStreamCount() ;i++)
    {
        createDecodeThread(m_decoderVec.at(i),i);
    }
    // 创建渲染线程
    createRenderThread();
    setScale(MediaData::getInstance()->getScale());
}

void MediaFunction::createDemuxThread(IUNknown *pUNknown)
{
    auto callback_recvPacket = std::bind(&MediaFunction::recvPacket,this,std::placeholders::_1);
    auto callback_clearPacket = std::bind(&MediaFunction::clearPacket,this);
    m_pDemuxThread = new DemuxThread(m_pMediaFac,pUNknown,callback_recvPacket);
    if(!m_pDemuxThread)
    {
        std::cout << "Failed to create a DemuxThread."<<std::endl;
    }
    m_pDemuxThread->clearPacket(callback_clearPacket);
    m_pDemuxThread->start();
}

void MediaFunction::createDecodeThread(IUNknown *pDecoderUnkown, int stream_index)
{
    auto callback_recvFrame = std::bind(&MediaFunction::recvFrame,this,std::placeholders::_1);
    auto callback_getPacket = std::bind(&MediaFunction::getPacket,this,std::placeholders::_1);
    DecodeThread *thread = new DecodeThread(m_pMediaFac,pDecoderUnkown,callback_getPacket,callback_recvFrame,stream_index);
    if(!thread)
    {
        std::cout << "Failed to create a DecodeThread";
    }
    thread->start();
    m_DecodeThreadVec.push_back(thread);
}

void MediaFunction::createRenderThread()
{
    if(MediaData::getInstance()->getVStramIndex() != -1)
    {
        auto callback_getFrontFrame = std::bind(&MediaFunction::getFrontFrame,this,std::placeholders::_1);
        auto callback_processVideo = std::bind(&MediaFunction::processVideo,this,std::placeholders::_1);
        auto callback_stopRender = std::bind(&MediaFunction::releaseRender,this);
        auto callback_getPacketCount = std::bind(&MediaFunction::getPacketCount,this,std::placeholders::_1);
        auto callback_positionChange = std::bind(&MediaFunction::positionChange,this,std::placeholders::_1);

        m_pVideoScaleUNknown = m_pMediaFac->createScaleUNknown();
        m_videoRenderThread = new RenderThread(m_pMediaFac,MediaData::getInstance()->getVStramIndex());
        if(!m_videoRenderThread)
        {
            std::cout<<"Failed to create a VideoRenderThread"<<std::endl;
        }
        m_videoRenderThread->setStreamCount(MediaData::getInstance()->getStreamCount());
        m_videoRenderThread->getFrontFrameCallBack(callback_getFrontFrame);
        m_videoRenderThread->sendFrameCallBack(callback_processVideo);
        m_videoRenderThread->stopRenderCallBack(callback_stopRender);
        m_videoRenderThread->getPacketCountCallBack(callback_getPacketCount);
        m_videoRenderThread->getPacketBufferSize(MediaData::getInstance()->getMaxPacketlistSize());
        m_videoRenderThread->positionChangeCallBack(callback_positionChange);
        if(MediaData::getInstance()->getAutoSpeed())
        {
            m_videoRenderThread->autoSpeed();
        }
        m_videoRenderThread->start();
    }
    if(MediaData::getInstance()->getAStramIndex() != -1)
    {
        auto callback_getFrontFrame = std::bind(&MediaFunction::getFrontFrame,this,std::placeholders::_1);
        auto callback_stopRender = std::bind(&MediaFunction::releaseRender,this);
        auto callback_processAudio = std::bind(&MediaFunction::processAudio,this,std::placeholders::_1);
        m_pAudioScaleUNknown = m_pMediaFac->createScaleUNknown();
        m_audioRenderThread = new RenderThread(m_pMediaFac,MediaData::getInstance()->getAStramIndex());
        if(!m_audioRenderThread)
        {
            std::cout<<"Failed to create a AudioRenderThread"<<std::endl;
        }

        m_audioRenderThread->getFrontFrameCallBack(callback_getFrontFrame);
        m_audioRenderThread->sendFrameCallBack(callback_processAudio);
        m_audioRenderThread->stopRenderCallBack(callback_stopRender);
        m_audioRenderThread->start();
    }
    if(MediaData::getInstance()->getSStramIndex() != -1)
    {
        std::cout << "subtitle frame pts is: "<< std::endl;
    }
}

void MediaFunction::recvPacket(IUNknown *pUNknown)
{
    // fyf: IUNknow对应的流索引放入对应队列,非放入全部放入(结束帧放入每个队列中)
    // 此处需要控制IUNKnown应用计数(放入一次+1),调用完成该函数减少引用计数

    IPtr<IPacket, &IID_IPACKET> spIPacket(pUNknown);
    if (!spIPacket)
    {
        return;
    }
    int stream_index = spIPacket->getStreamIndex();
    if(!spIPacket->isEnd())
    {
        if(m_packetQueue.push(stream_index,pUNknown))
        {
           pUNknown->AddRef();
        }
    }
    else
    {
        for(int i = 0;i<MediaData::getInstance()->getStreamCount();i++)
        {
            if(m_packetQueue.push(i,pUNknown))
            {
                pUNknown->AddRef();
            }
        }
    }

    int nAudioIndex = MediaData::getInstance()->getAStramIndex();
    int nVideoIndex = MediaData::getInstance()->getVStramIndex();
    int nVideoSize = -1;
    int nAudioSize = -1;
    if(nAudioIndex != -1)
    {
        nAudioSize = getPacketCount(nAudioIndex);
//        std::cout<<"nAudioSize:"<<nAudioSize<<std::endl;
    }
    if(nVideoIndex != -1)
    {
        nVideoSize = getPacketCount(nVideoIndex);
//        std::cout<<"nVideoSize:"<<nVideoSize<<std::endl;
    }

    MediaData::PlayStatus status = MediaData::PlayStatus::Normal;
    status = MediaData::getInstance()->judgePlayStatus(nVideoSize,nAudioSize);

    if(status == MediaData::PlayStatus::Pause)
    {
        stopRender();
        std::cout<<"------stopRender------"<<std::endl;
    }
    else if(status == MediaData::PlayStatus::Play)
    {
        startRender();
        std::cout<<"------startRender-----"<<std::endl;
    }

}

void MediaFunction::recvFrame(IUNknown *pUNknown)
{
    // fyf: IUNknow对应的流索引放入对应队列,非放入全部放入(结束帧放入每个队列中)
    // 此处需要控制IUNKnown应用计数(放入一次+1),调用完成该函数减少引用计数

    IPtr<IFrame, &IID_IFRAME> spIFrame(pUNknown);
    if (!spIFrame)
    {
        return;
    }

    IFrame::AVMediaType mediaType = spIFrame->getFrameType();
    if(mediaType == IFrame::AVMediaType::AVTypeVideo)
    {
        int FrameRate = spIFrame->getFrameRate();
        MediaData::getInstance()->setFrameRate(FrameRate);
    }

    int stream_index = spIFrame->getStreamIndex();
    {
        if(m_frameQueue.push(stream_index,pUNknown))
        {
            pUNknown->AddRef();
        }
    }
}

void MediaFunction::initIUNknownQueue()
{
    m_packetQueue.resize(MediaData::getInstance()->getStreamCount());
    m_packetQueue.init(MediaData::getInstance()->getStreamCount(),MediaData::getInstance()->getMaxPacketlistSize());
    m_frameQueue.resize(MediaData::getInstance()->getStreamCount());
    m_frameQueue.init(MediaData::getInstance()->getStreamCount(),MediaData::getInstance()->getMaxFramelistSize());
}

void MediaFunction::clearPacket()
{
    for(int i=0; i<MediaData::getInstance()->getStreamCount(); i++)
    {
        m_packetQueue.forcewake(i);
        while(!m_packetQueue.isEmpty(i))
        {
            m_packetQueue.pop(i)->Release();
        }
    }
}

void MediaFunction::clearFrame()
{
    for(int i=0; i<MediaData::getInstance()->getStreamCount(); i++)
    {
        m_frameQueue.forcewake(i);
        while(!m_frameQueue.isEmpty(i))
        {
            m_frameQueue.pop(i)->Release();
        }
        IUNknown* pDecoderUnknow = m_decoderVec[i];
        IPtr<IDecode, &IID_IDECODE> spIDecode(pDecoderUnknow);
        if(!spIDecode)
        {
            continue;
        }

        IUNknown* pPacketUnknow = m_pMediaFac->createPacketUNknown();
        IUNknown* pFrameUnknow = m_pMediaFac->createFrameUNknown();

        IDecode::IDecodeError errorCode = IDecode::NoError;

        IPtr<IPacket, &IID_IPACKET> spIPacket(pPacketUnknow);
        spIPacket->setEofFlag(true);
        while(errorCode != IDecode::DecodeEof)
        {
            errorCode = spIDecode->decodeData(pPacketUnknow,pFrameUnknow);
            pFrameUnknow->Release();
            pFrameUnknow = m_pMediaFac->createFrameUNknown();
        }
        if(pPacketUnknow)
        {
            pPacketUnknow->Release();
        }
        if(pFrameUnknow)
        {
            pFrameUnknow->Release();
        }
    }
}

void MediaFunction::clearData()
{
    m_pCallBack->clearRenderer();

    while(!m_decoderVec.empty())
    {
        IUNknown* pDecoder =  m_decoderVec.at(m_decoderVec.size()-1);
        if(pDecoder)
            pDecoder->Release();
        m_decoderVec.pop_back();
    }
    while(!m_streamVec.empty())
    {
        IUNknown* pStream =  m_streamVec.at(m_streamVec.size()-1);
        if(pStream)
            pStream->Release();
        m_streamVec.pop_back();
    }
    if(m_pDemuxerUnknow)
    {
        m_pDemuxerUnknow->Release();
        m_pDemuxerUnknow = NULL;
    }
    if(m_pAudioScaleUNknown)
    {
        m_pAudioScaleUNknown->Release();
        m_pAudioScaleUNknown = NULL;
    }
    if(m_pVideoScaleUNknown)
    {
        m_pVideoScaleUNknown->Release();
        m_pVideoScaleUNknown = NULL;
    }
}

void MediaFunction::Play()
{
    if(m_pEntrance->isRunning())
        return;
    stop();
    m_pEntrance->start();
}

void MediaFunction::stop()
{
    if(m_pEntrance->isRunning())
        return;

    if(!m_pDemuxThread)
        return;

    stopRender();
    m_pDemuxThread->stop();
    for(unsigned int i = 0; i < m_DecodeThreadVec.size(); i++)
    {
        DecodeThread* pThread = m_DecodeThreadVec.at(i);
        pThread->stop();
    }

    m_pDemuxThread->forceWakePause();
    forcewakeQueue();

    std::cout << "demuxer thread id: " << m_pDemuxThread->getThreadID() << std::endl;
    m_pDemuxThread->wait();
    delete m_pDemuxThread;
    m_pDemuxThread = nullptr;
    for(unsigned int i = 0; i<m_DecodeThreadVec.size(); i++)
    {
        DecodeThread* pThread = m_DecodeThreadVec.at(i);
        pThread->wait();
        delete pThread;
    }
    m_DecodeThreadVec.clear();

    if(m_audioRenderThread)
    {
        m_audioRenderThread->wait();
        if(m_audioRenderThread->getThreadID() == std::this_thread::get_id())
        {
            m_audioRenderThread->needClearSelf();
            m_audioRenderThread = nullptr;
        }
        else
        {
            delete m_audioRenderThread;
            m_audioRenderThread = nullptr;
        }
    }
    if(m_videoRenderThread)
    {
        m_videoRenderThread->wait();
        if(m_videoRenderThread->getThreadID() == std::this_thread::get_id())
        {
            m_videoRenderThread->needClearSelf();
            m_videoRenderThread = nullptr;
        }
        else
        {
            delete m_videoRenderThread;
            m_videoRenderThread = nullptr;
        }
    }

    clearPacket();
    clearFrame();
    clearData();
    MediaData::getInstance()->clearStreamIndex();
    playStop();

}

void MediaFunction::resume()
{
    startRender();
    if(m_pDemuxThread)
    {
        m_pDemuxThread->resume();
    }
}

bool MediaFunction::seek(int percent)
{
    bool bSeek = false;
    if (m_pDemuxThread)
    {
        m_pDemuxThread->stop();
        m_pDemuxThread->wait();
        bSeek = m_pDemuxThread->seek(percent);
        m_pDemuxThread->start();
    }
    return bSeek;
}

void MediaFunction::setScale(float scale)
{
     MediaData::getInstance()->setScale(scale);
    if(m_pDemuxThread)
    {
        m_pDemuxThread->setScale(scale);
    }
}

void MediaFunction::setSpeed(float fSpeed)
{
    MediaData::getInstance()->setSpeed(fSpeed);
    if(m_videoRenderThread)
    {
        m_videoRenderThread->setSpeed(fSpeed);
    }
}

void MediaFunction::pauseDemux()
{
    if(m_pDemuxThread)
    {
        m_pDemuxThread->pause();
    }
}

void MediaFunction::forcewakeQueue()
{
    for(int i=0; i<MediaData::getInstance()->getStreamCount(); i++)
    {
        m_packetQueue.forcewake(i);
        m_frameQueue.forcewake(i);
    }
}

int MediaFunction::getPacketCount(int nIndex)
{
    if(m_packetQueue.isEmpty(nIndex))
        return 0;
    return m_packetQueue.size(nIndex);
}

int64_t MediaFunction::Duration()
{
    if(!m_pDemuxerUnknow)
    {
        return 0;
    }
    IPtr<IDemux, &IID_IDEMUX> spIDemux(m_pDemuxerUnknow);
    if (!spIDemux)
    {
        std::cout<<"Cannot create a smart pointer to IDmux.";
        return 0;
    }
    return spIDemux->duration();
}

int64_t MediaFunction::getCoderate()
{
   if(m_videoRenderThread)
   {
       return m_videoRenderThread->getCodeRate();
   }
   return 0;
}

void MediaFunction::setCodeRateSecond(int nSecond)
{
    RenderParamControl::getInstance()->setCodeRateSecond(nSecond);
}

void MediaFunction::setClockType(IPlay::ClockType type)
{
    RenderParamControl::getInstance()->setClockType(type);
}

void MediaFunction::setMaxAutoSpeed(float nSpeed)
{
    RenderParamControl::getInstance()->setMaxAutoSpeed(nSpeed);
}

void MediaFunction::SetFileName(const char *fileName)
{
    MediaData::getInstance()->setFilename(fileName);
}

char *MediaFunction::GetFileName()
{
    return (char*)MediaData::getInstance()->getFilename().c_str();
}

MediaCallBack *MediaFunction::getCallBack()
{
    return m_pCallBack;
}

int64_t MediaFunction::readData(unsigned char *buffer, int len)
{
    if(m_pCallBack)
    {
        return m_pCallBack->readData(buffer,len);
    }
    return 0;
}

int64_t MediaFunction::seekData(int64_t offset, int whence)
{
    if(m_pCallBack)
    {
        return m_pCallBack->seekData(offset,whence);
    }
    return 0;
}

IUNknown *MediaFunction::getPacket(int index)
{
    return m_packetQueue.pop(index);
}

IUNknown *MediaFunction::getFrontFrame(int index)
{
    return m_frameQueue.pop(index);
}

void MediaFunction::releaseRender()
{
    MediaData::getInstance()->addReleaseIndex();
    if(MediaData::getInstance()->getReleaseIndex() == MediaData::getInstance()->getStreamCount())
    {
        if(m_pCallBack)
        {
            m_pCallBack->playFinsh();
        }
        stop();
        MediaData::getInstance()->resetReleaseIndex();
    }
}

void MediaFunction::getAudioFormat(int &sampleRate, int &aduioChannels, int &aduioFormat)
{
    MediaData::getInstance()->getAudioFormat(sampleRate,aduioChannels,aduioFormat);
}

void MediaFunction::setAudioData(const unsigned char *pAudioData, int nAudioSize)
{
    if(m_pCallBack)
    {
        m_pCallBack->setAudioData(pAudioData,nAudioSize);
    }
}

void MediaFunction::createVideoFrameFinish()
{
    if(m_pCallBack)
    {
        m_pCallBack->createVideoFrameFinish();
    }
}

void MediaFunction::positionChange(int position)
{
    if(m_pCallBack)
    {
        m_pCallBack->positionChange(position);
    }
}

unsigned char *MediaFunction::getVideoBuffer()
{
    if(m_pCallBack)
    {
        return m_pCallBack->getVideoBuffer();
    }
    return NULL;
}

void MediaFunction::getVideoFormat(int &nWidth, int &nHeight, int &nFormat)
{
    MediaData::getInstance()->getVideoFormat(nWidth,nHeight,nFormat);
}

bool MediaFunction::getRenderIsRunning()
{
    if(m_audioRenderThread)
    {
        if(m_audioRenderThread->isRunning() )
        {
            return true;
        }
    }
    if(m_videoRenderThread)
    {
        if(m_videoRenderThread->isRunning())
        {
            return true;
        }
    }
    return false;
}

void MediaFunction::stopRender()
{
    if(m_audioRenderThread)
    {
        m_audioRenderThread->stop();
    }
    if(m_videoRenderThread)
    {
        m_videoRenderThread->stop();
    }
}

void MediaFunction::startRender()
{
    if(m_audioRenderThread)
    {
        m_audioRenderThread->start();
    }
    if(m_videoRenderThread)
    {
        m_videoRenderThread->start();
    }
}

void MediaFunction::playStop()
{
    if(m_pCallBack)
    {
        m_pCallBack->playStop();
    }
}

void MediaFunction::processAudio(IUNknown *pFrameUNknown)
{
    IPtr<IFrame, &IID_IFRAME> spIFrame(pFrameUNknown);
    int nSampleRate = -1;
    int nChannelCount = -1;
    int nSampleFormat = -1;
    getAudioFormat(nSampleRate,nChannelCount,nSampleFormat);
    if(nSampleFormat != -1)
    {
        IPtr<IScale, &IID_ISCALE> spIScale(m_pAudioScaleUNknown);
        IScale::IScaleError error = spIScale->converAudio(pFrameUNknown, nChannelCount, nSampleRate, (IScale::AudioSampleFormat)nSampleFormat);
        if(error != IScale::NoError)
        {
            return ;
        }
    }
    int nAudioSize = 0;
    const unsigned char* pAudioData = spIFrame->getFrameData(nAudioSize);
    setAudioData(pAudioData,nAudioSize);
    std::cout << "Audio pts is: " << spIFrame->getPts() << std::endl;
}

void MediaFunction::processVideo(IUNknown *pFrameUNknown)
{
    int nWidth = -1;
    int nHeight = -1;
    int nFormat = -1;
    getVideoFormat(nWidth,nHeight,nFormat);

    unsigned char* pAllocData = getVideoBuffer();
    if(nFormat != IScale::FormatNone)
    {
        IPtr<IScale, &IID_ISCALE> spIScale(m_pVideoScaleUNknown);
        IScale::IScaleError error = spIScale->convertImg(pFrameUNknown, pAllocData, (IScale::FrameFormat)nFormat, nWidth, nHeight);
        if(error != IScale::NoError)
        {
            return ;
        }
        QRFilter::ImgRect imgRect{0, 0, nWidth, nHeight};
        m_qrcode.addQRcode(pAllocData, QRFilter::IMG_YUV420, imgRect);
    }
    createVideoFrameFinish();
    IPtr<IFrame, &IID_IFRAME> spIFrame(pFrameUNknown);
}

void MediaFunction::setQRCodeText(std::string strText)
{
    m_qrcode.setText(strText);
}


