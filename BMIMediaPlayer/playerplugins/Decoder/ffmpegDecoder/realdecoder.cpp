extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}
#include "realdecoder.h"
#include "audiodecoder.h"
#include "videodecoder.h"
#include "subitledecoder.h"
#include <packet.h>
#include <stream.h>
#include <frame.h>

RealDecoder::RealDecoder(AVCodecContext *pCodecContext, IUNknown *pIStream)
{
    m_pCodecContext.reset(pCodecContext, [&](AVCodecContext *pContext){
        avcodec_close(pContext);
        avcodec_free_context(&pContext);
    });
    m_pIStream.reset(pIStream, [&](IUNknown *pIStream) {
        pIStream->Release();
    });
    m_pIStream->AddRef();
}

RealDecoder::~RealDecoder()
{
    m_pCodecContext.reset();
    m_pIStream.reset();
}

RealDecoder *RealDecoder::CreateDecoder(AVCodecContext *pCodecContext, IUNknown *pIStream)
{
    switch (pCodecContext->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        return new VideoDecoder(pCodecContext, pIStream);
    case AVMEDIA_TYPE_AUDIO:
        return new AudioDecoder(pCodecContext, pIStream);
    case AVMEDIA_TYPE_SUBTITLE:
        return new SubitleDecoder(pCodecContext, pIStream);
    default:
        break;
    }
    return nullptr;
}

void RealDecoder::getPacket(AVPacket *pkt, IUNknown *pIPacket, IUNknown *pIStream)
{
    if(pkt == nullptr)
        return;
    IPtr<IPacket, &IID_IPACKET> spIPacket(pIPacket);
    IPtr<IAVStream, &IID_ISTREAM> spIStream(pIStream);
    IAVStream::StreamRational timebase = spIStream->getTimebase();
    pkt->dts = spIPacket->getDts() * timebase.den / timebase.num /1000.0;
    pkt->pts = spIPacket->getPts() * timebase.den / timebase.num /1000.0;
    pkt->pos = spIPacket->getPacketPos();
    pkt->flags = spIPacket->getPacketFlag();
    pkt->duration = spIPacket->getPacketDuration();
    pkt->stream_index = spIPacket->getStreamIndex();
    int dataLen = 0;
    const unsigned char* pData = spIPacket->getPacketData(dataLen);

    if(dataLen > 0)
    {
        pkt->data = (uint8_t *)pData;
        pkt->size = dataLen;
    }
}
