extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}
#include "subitledecoder.h"
#include <packet.h>
#include <frame.h>
#include <stream.h>

SubitleDecoder::SubitleDecoder(AVCodecContext *pCodecContext, IUNknown *pIStream)
    : RealDecoder(pCodecContext, pIStream)
{

}

SubitleDecoder::~SubitleDecoder()
{

}

IDecode::IDecodeError SubitleDecoder::decodeData(IUNknown *pIPacket, IUNknown *pIFrame)
{
    IPtr<IPacket, &IID_IPACKET> spIPacket(pIPacket);
    IPtr<IFrame, &IID_IFRAME> spIFrame(pIFrame);
    IPtr<IAVStream, &IID_ISTREAM> spIStream(m_pIStream.get());
    if(!spIPacket || !spIFrame)
        return IDecode::ParamError;
    if(spIPacket->packetName() != "FFMPEG PACKET")
        return IDecode::ParamError;

    AVPacket* pPacket = av_packet_alloc();
    if(!spIPacket->isEnd())
        getPacket(pPacket, pIPacket, m_pIStream.get());

    int nDecompressedFlag = 0;
    int ret = -1;

    AVSubtitle *subtitle = (AVSubtitle*)av_malloc(sizeof(AVSubtitle));
    ret = avcodec_decode_subtitle2(m_pCodecContext.get(), subtitle, &nDecompressedFlag, pPacket);
    av_packet_free(&pPacket);
    if(ret <= 0 && spIPacket->isEnd())
    {
        spIFrame->setEofFlag(true);
        spIFrame->setStreamIndex(spIStream->getStreamIndex());
        return IDecode::DecodeEof;
    }
    if(ret < 0)
        return IDecode::DecodeError;
    if(nDecompressedFlag == 0)
        return IDecode::FrameUseless;

    auto releaseFunc = [&](void* pSubtitle)
    {
        AVSubtitle* frame = (AVSubtitle*)pSubtitle;
        avsubtitle_free(frame);
        av_freep(&frame);
        frame = nullptr;
    };
    spIFrame->setPrivateData(subtitle, releaseFunc);
    spIFrame->setPts(subtitle->pts * 1000 / AV_TIME_BASE);
    spIFrame->setStreamIndex(spIStream->getStreamIndex());
    spIFrame->setFrameType(IFrame::AVTypeSubtitle);
    return IDecode::NoError;
}

