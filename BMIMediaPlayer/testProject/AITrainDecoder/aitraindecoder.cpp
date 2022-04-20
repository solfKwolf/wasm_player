#include "aitraindecoder.h"
#include <iostream>
#include <frame.h>
#include <scale.h>
#include <packet.h>
#include <demux.h>
#include <decode.h>
#include <frame.h>
#include <scale.h>
#include <stream.h>
#include <fstream>
#include <vector>
#include "savepicture.h"

AITrainDecoder::AITrainDecoder()
    : m_nFrameNum(0)
#ifdef __WIN32__
    , m_demuxerDelegate("ffmpegDemuxer.dll")
    , m_decoderDelegate("ffmpegDecoder.dll")
    , m_packetDelegate("ffmpegPacket.dll")
    , m_streamDelegate("ffmpegStream.dll")
    , m_frameDelegate("ffmpegFrame.dll")
    , m_scaleDelegate("ffmpegScale.dll")
#else
    , m_demuxerDelegate("libffmpegDemuxer.so")
    , m_decoderDelegate("libffmpegDecoder.so")
    , m_packetDelegate("libffmpegPacket.so")
    , m_streamDelegate("libffmpegStream.so")
    , m_frameDelegate("libffmpegFrame.so")
    , m_scaleDelegate("libffmpegScale.so")
#endif

{

}

AITrainDecoder::~AITrainDecoder()
{

}

bool AITrainDecoder::DecodeFile(std::string &url, int framenum, std::string &dstpath)
{
    IUNknown *pScaleUnknow = m_scaleDelegate.createUnknow();
    IUNknown *pDemuxerUnknow = m_demuxerDelegate.createUnknow();
    m_dstpath = dstpath;
    m_nFrameNum = framenum;
    if(!pDemuxerUnknow)
    {
        std::cout << "load ffmpeg demuxer error." << std::endl;
        return false;
    }

    IPtr<IDemux, &IID_IDEMUX> spIDemuxer(pDemuxerUnknow);
    if(spIDemuxer->initDemuxer(url) != IDemux::NoError)
    {
        std::cout << "init demuxer error." << std::endl;
        return false;
    }

    std::vector<IUNknown*> streamVec;
    std::vector<IUNknown*> decoderVec;

    int nStreamCount = spIDemuxer->getStreamCount();
    for(int i = 0; i < nStreamCount; i++)
    {
        IUNknown* pStreamUNknown = m_streamDelegate.createUnknow();
        spIDemuxer->getStream(i, pStreamUNknown);

        IUNknown* pDecoderUnknow = m_decoderDelegate.createUnknow();
        IPtr<IDecode, &IID_IDECODE> spIDecoder(pDecoderUnknow);
        IPtr<IAVStream, &IID_ISTREAM> spIStream(pStreamUNknown);
        std::cout << "stream title is: " << spIStream->getStreamTitle() << std::endl;
        IDecode::IDecodeError ret = spIDecoder->initDecoder(pStreamUNknown);
        if(ret == IDecode::CodecOpenError)
        {
            pStreamUNknown->Release();
            pDecoderUnknow->Release();
        }
        else
        {
            streamVec.push_back(pStreamUNknown);
            decoderVec.push_back(pDecoderUnknow);
        }
    }

    while(true)
    {
        IUNknown* pPacketUnknow = m_packetDelegate.createUnknow();
        IDemux::IDemuxError error = spIDemuxer->demuxStream(pPacketUnknow);
        if(error == IDemux::NoError)
        {
            IPtr<IPacket, &IID_IPACKET> spIPacket(pPacketUnknow);
            IUNknown* pFrameUnknow = m_frameDelegate.createUnknow();
            int ret = decode(pPacketUnknow, decoderVec.at(spIPacket->getStreamIndex()), pFrameUnknow, pScaleUnknow);
            if(ret ==-1)
            {
                pPacketUnknow->Release();
                pFrameUnknow->Release();
                break;
            }
            pFrameUnknow->Release();
        }
        else if(error == IDemux::DemuxerEof)
        {
            std::cout << "demux eof." << std::endl;
            // 清空解码器
            for(int i =0 ; i < spIDemuxer->getStreamCount(); i++)
            {
                IUNknown* pFrameUnknow = m_frameDelegate.createUnknow();
                while(decode(pPacketUnknow, decoderVec.at(i), pFrameUnknow, pScaleUnknow) == IDecode::NoError)
                {
                    pFrameUnknow->Release();
                    pFrameUnknow = m_frameDelegate.createUnknow();
                }
                pFrameUnknow->Release();
            }
            std::cout << "decodec eof." << std::endl;
            break;
        }
        else
        {
            std::cout << "demux error." << std::endl;
            break;
        }
        pPacketUnknow->Release();
    }

    pDemuxerUnknow->Release();
    pScaleUnknow->Release();

    for(int i = 0; i < decoderVec.size(); i++)
    {
        streamVec.at(i)->Release();
        decoderVec.at(i)->Release();
    }
    streamVec.clear();
    decoderVec.clear();
    return true;
}

int AITrainDecoder::decode(IUNknown *pPacketUnknow, IUNknown *pDecoderUnknow, IUNknown *pFrameUnknow, IUNknown *pScaleUnknow)
{
    IPtr<IDecode, &IID_IDECODE> spIDecoder(pDecoderUnknow);
    IDecode::IDecodeError errorCode = spIDecoder->decodeData(pPacketUnknow, pFrameUnknow);
    if(errorCode == IDecode::NoError)
    {
        IPtr<IFrame, &IID_IFRAME> spIFrame(pFrameUnknow);
        static int nName = 1;
        if(spIFrame->getFrameType() == IFrame::AVTypeVideo)
        {
            unsigned char* pAllocData = nullptr;
            if(spIFrame->getFrameFormat() != IFrame::FormatRGB24)
            {
                IPtr<IScale, &IID_ISCALE> spIScale(pScaleUnknow);
                pAllocData = new unsigned char[spIScale->getPictureSize(IScale::FormatRGB24, spIFrame->frameWidth(), spIFrame->frameHeight())];
                spIScale->convertImg(pFrameUnknow, pAllocData, IScale::FormatRGB24, spIFrame->frameWidth(), spIFrame->frameHeight());
            }
            if(m_nFrameNum != 0)
            {
                std::string filename = m_dstpath+std::to_string(nName++);
                int nImageSize = 0;
                const unsigned char* pData = spIFrame->getFrameData(nImageSize);

                SavePicture savePic ;
                savePic.rgb_to_jpeg(filename.c_str(), (unsigned char*)pData,spIFrame->frameWidth(), spIFrame->frameHeight(),100);
                if(pAllocData)
                    delete[] pAllocData;
                m_nFrameNum--;
                if(m_nFrameNum == 0)
                {
                    return -1;
                }
            }
            std::cout << "video frame pts is: " << spIFrame->getPts() << std::endl;
        }
    }
    else if(errorCode == IDecode::DecodeEof)
    {
        std::cout<<"decodec eof."<<std::endl;
    }
    return errorCode;
}

