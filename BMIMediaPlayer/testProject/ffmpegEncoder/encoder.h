#ifndef ENCODER_H
#define ENCODER_H
#include "cbasethread.h"
#include <iostream>
#include <packet.h>
#include <demux.h>
#include <decode.h>
#include <frame.h>
#include <stream.h>
#include <encode.h>
#include <mux.h>
#include <fstream>
#include <vector>
#include "comdelegate.h"

class Decoder : public CBaseThread
{
public:
    Decoder();
    ~Decoder();
    void setUrl(std::string strUrl, std::string outUrl, std::string outFormat);

    void start();
    void run();
    void stop();
    void finished();

    IDecode::IDecodeError decodePacket(IUNknown* pPacketUnknow, IUNknown* pFrameUnknow, IUNknown* pDecoderUnknow);
    IEncode::IEncodeError encodePacket(IUNknown* pPacketUnknow, IUNknown* pFrameUnknow, IUNknown* pEncoderUnknow);

private:
    bool m_isStop;
    std::string m_strUrl;
    std::string m_strOutUrl;
    std::string m_strOutFormat;
};

#endif // ENCODER_H
