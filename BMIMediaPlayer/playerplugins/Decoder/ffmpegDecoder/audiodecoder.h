#ifndef AUDIODECODER_H
#define AUDIODECODER_H
#include "realdecoder.h"
#include <frame.h>
#include <memory>

extern "C"
{
    struct AVFrame;
}

typedef std::shared_ptr<AVFrame> AVFramePtr;

class AudioDecoder : public RealDecoder
{
public:
    AudioDecoder(AVCodecContext *pCodecContext, IUNknown* pIStream);
    ~AudioDecoder();
public:
    virtual IDecode::IDecodeError decodeData(IUNknown* pIPacket, IUNknown* pIFrame) override;
private:
    AVFramePtr m_pFrame;
};

#endif // AUDIODECODER_H
