#ifndef SUBITLEDECODER_H
#define SUBITLEDECODER_H
#include "realdecoder.h"

class SubitleDecoder : public RealDecoder
{
public:
    SubitleDecoder(AVCodecContext *pCodecContext, IUNknown* pIStream);
    ~SubitleDecoder();
public:
    virtual IDecode::IDecodeError decodeData(IUNknown* pIPacket, IUNknown* pIFrame) override;
};

#endif // SUBITLEDECODER_H
