#ifndef REALDECODER_H
#define REALDECODER_H

#include <unknown.h>
#include <decode.h>
#include <memory>

extern "C"
{
    struct AVCodec;
    struct AVCodecContext;
    struct AVPacket;
    struct AVFrame;
}

typedef std::shared_ptr<AVCodecContext> AVCodecContextPtr;
typedef std::shared_ptr<IUNknown> IUNknownPtr;

class RealDecoder
{
public:
    RealDecoder(AVCodecContext *pCodecContext, IUNknown* pIStream);
    virtual ~RealDecoder();

    static RealDecoder* CreateDecoder(AVCodecContext *pCodecContext, IUNknown* pIStream);

public:
    void getPacket(AVPacket* pkt, IUNknown* pIPacket, IUNknown* pIStream);
    virtual IDecode::IDecodeError decodeData(IUNknown* pIPacket, IUNknown* pIFrame) = 0;
protected:
    AVCodecContextPtr m_pCodecContext;
    IUNknownPtr m_pIStream;
};

#endif // REALDECODER_H
