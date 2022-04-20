#include "decoder.h"

Decoder::Decoder()
{

}

Decoder::~Decoder()
{

}

void Decoder::setUrl(std::string strUrl)
{
    m_strUrl = strUrl;
}

IDecode::IDecodeError Decoder::decode(IUNknown* pPacketUnknow, IUNknown* pDecoderUnknow, IUNknown* pFrameUnknow, IUNknown *pScaleUnknow)
{
    IPtr<IDecode, &IID_IDECODE> spIDecoder(pDecoderUnknow);
    IDecode::IDecodeError errorCode = spIDecoder->decodeData(pPacketUnknow, pFrameUnknow);
    if(errorCode == IDecode::NoError)
    {
        IPtr<IFrame, &IID_IFRAME> spIFrame(pFrameUnknow);
        static int nName = 0;
        if(spIFrame->getFrameType() == IFrame::AVTypeVideo)
        {
            unsigned char* pAllocData = nullptr;
            if(spIFrame->getFrameFormat() != IFrame::FormatRGB24)
            {
                IPtr<IScale, &IID_ISCALE> spIScale(pScaleUnknow);
                pAllocData = new unsigned char[spIScale->getPictureSize(IScale::FormatRGB24, 60, 30)];
                spIScale->convertImg(pFrameUnknow, pAllocData, IScale::FormatRGB24, 60, 30);
            }
            int nImageSize = 0;
            const unsigned char* pData = spIFrame->getFrameData(nImageSize);
            int32_t *pARGB = (int32_t*)pData;
            QImage img ((unsigned char*)pARGB, spIFrame->frameWidth(), spIFrame->frameHeight(), 60 * 3,  QImage::Format_RGB888);
            QImage imgTemp = img.copy(img.rect());
            imgTemp.save("D:\\test\\img\\" + QString::number(nName++) + ".jpeg", "jpeg");
            if(pAllocData)
                delete[] pAllocData;

//            std::cout << "video frame pts is: " << spIFrame->getPts() << std::endl;
        }
        else if(spIFrame->getFrameType() == IFrame::AVTypeAudio)
        {
            if(!audioRender.isInit())
            {
                audioRender.setAudioFormat(spIFrame->getSampleRate(), spIFrame->getAduioChannels());
            }
            if(spIFrame->getAudioSampleFormat() != IFrame::AudioFormateS16)
            {
                IPtr<IScale, &IID_ISCALE> spIScale(pScaleUnknow);
                IScale::IScaleError error = spIScale->converAudio(pFrameUnknow, spIFrame->getAduioChannels(), spIFrame->getSampleRate(), IScale::AudioFormateS16);
                if(error != IScale::NoError)
                {
                    return IDecode::DecodeError;
                }
            }

            int nAudioSize = 0;
            const unsigned char* pAudioData = spIFrame->getFrameData(nAudioSize);
            audioRender.setData(QByteArray((const char*)pAudioData, nAudioSize));
            std::cout << "audio frame pts is: " << spIFrame->getPts() << std::endl;
        }
        else
        {
            std::cout << "subtitle frame pts is: " << spIFrame->getPts() << std::endl;
        }
    }
    else if(errorCode == IDecode::DecodeEof)
    {
        std::cout<<"-----------errorCode == IDecode::DecodeEof--------"<<std::endl;
    }
    return errorCode;
}

void Decoder::run()
{
#ifdef _WIN32
    COMDelegate demuxerDelegate("ffmpegDemuxer.dll");
    COMDelegate packetDelegate("ffmpegPacket.dll");
    COMDelegate decoderDelegate("ffmpegDecoder.dll");
    COMDelegate streamDelegate("ffmpegStream.dll");
    COMDelegate frameDelegate("ffmpegFrame.dll");
    COMDelegate scaleDelegate("ffmpegScale.dll");
#else
    COMDelegate demuxerDelegate("libffmpegDemuxer.so");
    COMDelegate packetDelegate("libffmpegPacket.so");
    COMDelegate decoderDelegate("libffmpegDecoder.so");
    COMDelegate streamDelegate("libffmpegStream.so");
    COMDelegate frameDelegate("libffmpegFrame.so");
    COMDelegate scaleDelegate("libffmpegScale.so");
#endif
    IUNknown *pScaleUnknow = scaleDelegate.createUnknow();
    IUNknownDeleter scaleDeleter(pScaleUnknow);

    IUNknown* pDemuxerUnknow = demuxerDelegate.createUnknow();
    if(!pDemuxerUnknow)
    {
        std::cout << "load ffmpeg demuxer error." << std::endl;
        return;
    }
    IUNknownDeleter deleter(pDemuxerUnknow);

    IPtr<IDemux, &IID_IDEMUX> spIDemuxer(pDemuxerUnknow);

    if(spIDemuxer->initDemuxer(m_strUrl) != IDemux::NoError)
    {
        std::cout << "init demuxer error." << std::endl;
        return;
    }

    std::vector<IUNknown*> streamVec;
    std::vector<IUNknown*> decoderVec;
    int nStreamCount = spIDemuxer->getStreamCount();
    for(int i = 0; i < nStreamCount; i++)
    {
        IUNknown* pUNknow = streamDelegate.createUnknow();
        spIDemuxer->getStream(i, pUNknow);

        IUNknown* pDecoderUnknow = decoderDelegate.createUnknow();
        IPtr<IDecode, &IID_IDECODE> spIDecoder(pDecoderUnknow);
        IPtr<IAVStream, &IID_ISTREAM> spIStream(pUNknow);
        std::cout << "stream title is: " << spIStream->getStreamTitle() << std::endl;
        spIDecoder->setHardware(true);
        spIDecoder->initDecoder(pUNknow);

        streamVec.push_back(pUNknow);
        decoderVec.push_back(pDecoderUnknow);
    }

    while(true)
    {
        IUNknown* pPacketUnknow = packetDelegate.createUnknow();
        IDemux::IDemuxError error = spIDemuxer->demuxStream(pPacketUnknow);
        if(error == IDemux::NoError)
        {
            IPtr<IPacket, &IID_IPACKET> spIPacket(pPacketUnknow);
            IUNknown* pFrameUnknow = frameDelegate.createUnknow();
            decode(pPacketUnknow, decoderVec.at(spIPacket->getStreamIndex()), pFrameUnknow, pScaleUnknow);
            pFrameUnknow->Release();
        }
        else if(error == IDemux::DemuxerEof)
        {
            std::cout << "demux eof." << std::endl;
            // 清空解码器
            IPtr<IPacket, &IID_IPACKET> spIPacket(pPacketUnknow);
            for(int i =0 ; i < spIDemuxer->getStreamCount(); i++)
            {
                IUNknown* pFrameUnknow = frameDelegate.createUnknow();
                while(decode(pPacketUnknow, decoderVec.at(i), pFrameUnknow, pScaleUnknow) == IDecode::NoError)
                {
                    pFrameUnknow->Release();
                    pFrameUnknow = frameDelegate.createUnknow();
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

    for(int i = 0; i < nStreamCount; i++)
    {
        streamVec.at(i)->Release();
        decoderVec.at(i)->Release();
    }
    streamVec.clear();
    decoderVec.clear();
}

void Decoder::finished()
{
    std::cout << "over..." << std::endl;
}

