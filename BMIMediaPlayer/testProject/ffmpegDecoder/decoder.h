#ifndef DECODER_H
#define DECODER_H
#include "cbasethread.h"
#include "audiorender.h"
#include <iostream>
#include <QImage>
#include <packet.h>
#include <demux.h>
#include <decode.h>
#include <frame.h>
#include <scale.h>
#include <stream.h>
#include <fstream>
#include <vector>
#include "comdelegate.h"

class Decoder : public CBaseThread
{
public:
    Decoder();
    ~Decoder();
    void setUrl(std::string strUrl);

    void run();
    void finished();

    AudioRender audioRender;

    IDecode::IDecodeError decode(IUNknown* pPacketUnknow, IUNknown* pDecoderUnknow, IUNknown* pFrameUnknow, IUNknown *pScaleUnknow);

private:
    std::string m_strUrl;
};

#endif // DECODER_H
