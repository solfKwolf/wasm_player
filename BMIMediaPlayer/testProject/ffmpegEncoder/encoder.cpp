#include "encoder.h"

Decoder::Decoder()
{

}

Decoder::~Decoder()
{

}

void Decoder::setUrl(std::string strUrl, std::string outUrl, std::string outFormat)
{
    m_strUrl = strUrl;
    m_strOutUrl = outUrl;
    m_strOutFormat = outFormat;
}

void Decoder::start()
{
    m_isStop = false;
    CBaseThread::start();
}

IDecode::IDecodeError Decoder::decodePacket(IUNknown* pPacketUnknow, IUNknown* pFrameUnknow, IUNknown* pDecoderUnknow)
{
    IPtr<IDecode, &IID_IDECODE> spIDecoder(pDecoderUnknow);
    IDecode::IDecodeError errorCode = spIDecoder->decodeData(pPacketUnknow, pFrameUnknow);
    if(errorCode == IDecode::NoError)
    {
        IPtr<IFrame, &IID_IFRAME> spIFrame(pFrameUnknow);
        std::cout << "video frame pts is: " << spIFrame->getPts() << std::endl;
    }
    else if(errorCode == IDecode::DecodeEof)
    {
        std::cout<<"-----------errorCode == IDecode::DecodeEof--------"<<std::endl;
    }
    return errorCode;
}

IEncode::IEncodeError Decoder::encodePacket(IUNknown *pPacketUnknow, IUNknown *pFrameUnknow, IUNknown *pEncoderUnknow)
{
    IPtr<IEncode, &IID_IENCODE> spIEncoder(pEncoderUnknow);
    IEncode::IEncodeError errorCode = spIEncoder->encodeData(pFrameUnknow, pPacketUnknow);
    if (errorCode == IEncode::NoError)
    {
        IPtr<IPacket, &IID_IPACKET> spIPacket(pPacketUnknow);
        std::cout << "video frame pts is: " << spIPacket->getPts() << std::endl;
    }
    else if(errorCode == IEncode::EncoderEof)
    {
        std::cout<<"-----------errorCode == IDecode::EncodeEof--------"<<std::endl;
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
    COMDelegate encoderDelegate("ffmpegEncoder.dll");
    COMDelegate muxerDelegate("ffmpegMuxer.dll");
#else
    COMDelegate demuxerDelegate("libffmpegDemuxer.so");
    COMDelegate packetDelegate("libffmpegPacket.so");
    COMDelegate decoderDelegate("libffmpegDecoder.so");
    COMDelegate streamDelegate("libffmpegStream.so");
    COMDelegate frameDelegate("libffmpegFrame.so");
    COMDelegate scaleDelegate("libffmpegScale.so");
    COMDelegate encoderDelegate("libffmpegEncoder.so");
    COMDelegate muxerDelegate("libffmpegMuxer.so");
#endif

    // init demuxer
    IUNknown* pDemuxerUnknow = demuxerDelegate.createUnknow();
    if(!pDemuxerUnknow)
    {
        std::cout << "load ffmpeg demuxer error." << std::endl;
        return;
    }

    IPtr<IDemux, &IID_IDEMUX> spIDemuxer(pDemuxerUnknow);

    if(spIDemuxer->initDemuxer(m_strUrl) != IDemux::NoError)
    {
        std::cout << "init demuxer error." << std::endl;
        spIDemuxer->Release();
        return;
    }

    // init decoder
    std::vector<IUNknown*> streamVec;
    IUNknown* videoDecoder = decoderDelegate.createUnknow();
    int nStreamCount = spIDemuxer->getStreamCount();
    for(int i = 0; i < nStreamCount; i++)
    {
        IUNknown* pUNknow = streamDelegate.createUnknow();
        spIDemuxer->getStream(i, pUNknow);

        IPtr<IAVStream, &IID_ISTREAM> spIStream(pUNknow);
        std::cout << "stream title is: " << spIStream->getStreamTitle() << std::endl;
        streamVec.push_back(pUNknow);

        if(spIStream->getStreamType() == IAVStream::AVTypeVideo)
        {
            IPtr<IDecode, &IID_IDECODE> spIDecoder(videoDecoder);
            spIDecoder->setHardware(true);
            spIDecoder->initDecoder(pUNknow);
        }
    }

    // init encoder
    std::vector<IUNknown*> outStreamVec;
    IUNknown *videoEncoder = encoderDelegate.createUnknow();
    for(int i = 0; i < nStreamCount; i++)
    {
        IUNknown* pUNknow = streamVec.at(i);
        IPtr<IAVStream, &IID_ISTREAM> spIStream(pUNknow);
        if(spIStream->getStreamType() == IAVStream::AVTypeVideo)
        {
            IUNknown* pUNknowOut = streamDelegate.createUnknow();
            IPtr<IAVStream, &IID_ISTREAM> spIStreamOut(pUNknowOut);
            outStreamVec.push_back(pUNknowOut);

            std::cout << "current bit rate" << spIStream->getBitrate() << std::endl;
            spIStreamOut->setBitrate(spIStream->getBitrate());
            spIStreamOut->setStreamType(spIStream->getStreamType());
            spIStreamOut->setStreamCodecType(IAVStream::CodecTypeHevc);
            spIStreamOut->setStreamIndex(spIStream->getStreamIndex());
            int width, height;
            spIStream->getFrameSize(width, height);
            spIStreamOut->setFrameSize(width, height);
            spIStreamOut->setFrameFormat(spIStream->getFrameFormat());
            spIStreamOut->setTimebase(spIStream->getTimebase());
            spIStreamOut->setFrameRate(spIStream->getFrameRate());

            IPtr<IEncode, &IID_IENCODE> spIEncoder(videoEncoder);
//            spIEncoder->setBitrate(102400);
            spIEncoder->setBitrate(51200);
            spIEncoder->setGopSize(100);
            spIEncoder->setMaxQuality(40);
            spIEncoder->setMinQuality(36);
            spIEncoder->initEncoder(spIStreamOut, "hevc_nvenc");
        }
        else
        {
            spIStream->AddRef();
            outStreamVec.push_back(pUNknow);
        }
    }

    // init muxer
    IUNknown* pMuxerUnknow = muxerDelegate.createUnknow();

    IPtr<IMux, &IID_IMUX> spIMuxer(pMuxerUnknow);

    if(spIMuxer->initMuxer(m_strOutUrl, m_strOutFormat) != IMux::NoError)
    {
        std::cout << "init muxer error." << std::endl;
        goto releaseAll;
    }

    for (int i = 0; i < (int)outStreamVec.size(); i++)
    {
        spIMuxer->addStream(outStreamVec.at(i));
    }
    if(spIMuxer->writeHeader() != IMux::NoError)
    {
        std::cout << "write header error" << std::endl;
        goto releaseAll;
    }

    while(!m_isStop)
    {
        IUNknown* pPacketUnknow = packetDelegate.createUnknow();
        IDemux::IDemuxError error = spIDemuxer->demuxStream(pPacketUnknow);
        bool isValidPacket = true;
        if(error == IDemux::NoError)
        {
            IPtr<IPacket, &IID_IPACKET> spIPacket(pPacketUnknow);

            if(spIPacket->getPacketType() == IPacket::AVTypeVideo)
            {
                isValidPacket = false;
                IUNknown* pFrameUnknow = frameDelegate.createUnknow();
                if (decodePacket(pPacketUnknow, pFrameUnknow, videoDecoder) == IDecode::NoError)
                {
                    spIPacket->resetPrivateData();
                    isValidPacket = (encodePacket(pPacketUnknow, pFrameUnknow, videoEncoder) == IEncode::NoError);
                    pFrameUnknow->Release();
                }
            }
        }
        else if(error == IDemux::DemuxerEof)
        {
            std::cout << "demux eof." << std::endl;
            // 清空解码器
            IPtr<IPacket, &IID_IPACKET> spIPacket(pPacketUnknow);
            IUNknown* pFrameUnknow = frameDelegate.createUnknow();
            IPtr<IFrame, &IID_IFRAME> spIFrame(pFrameUnknow);
            while(decodePacket(pPacketUnknow, pFrameUnknow, videoDecoder) == IDecode::NoError)
            {
                spIPacket->resetPrivateData();
                if(encodePacket(pPacketUnknow, pFrameUnknow, videoEncoder) == IEncode::NoError)
                {
                    spIMuxer->muxPacket(pPacketUnknow);
                }
                spIPacket->resetPrivateData();
                spIPacket->setEofFlag(true);
                spIFrame->resetPrivateData();
            }
            std::cout << "decodec eof." << std::endl;

            // 清空编码器
            spIFrame->setEofFlag(true);
            while(encodePacket(pPacketUnknow, pFrameUnknow, videoEncoder) == IEncode::NoError)
            {
                spIMuxer->muxPacket(pPacketUnknow);
                spIPacket->resetPrivateData();
            }
            std::cout << "encode eof." << std::endl;

            pFrameUnknow->Release();
            pPacketUnknow->Release();
            spIMuxer->writeTrailer();
            break;
        }
        else
        {
            std::cout << "demux error." << std::endl;
            break;
        }
        if (isValidPacket)
            spIMuxer->muxPacket(pPacketUnknow);
        pPacketUnknow->Release();
    }

releaseAll:
    for(int i = 0; i < nStreamCount; i++)
    {
        outStreamVec.at(i)->Release();
    }
    outStreamVec.clear();
    pMuxerUnknow->Release();
    videoEncoder->Release();
    videoDecoder->Release();
    for(int i = 0; i < nStreamCount; i++)
    {
        streamVec.at(i)->Release();
    }
    streamVec.clear();
    pDemuxerUnknow->Release();
    std::cout << "run over..." << std::endl;

}

void Decoder::stop()
{
    m_isStop = true;
}

void Decoder::finished()
{
    std::cout << "over..." << std::endl;
}

