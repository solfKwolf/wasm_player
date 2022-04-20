#ifndef WEBDECODER_H
#define WEBDECODER_H

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include "../BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/decoderffmpeg.h"
#include "../webresource/webpacket.h"
#include "../webresource/webstream.h"
#include "../webresource/webframe.h"

using namespace emscripten;

class WebDecoderFFmpeg : public DecoderFFmpeg {
public:
    WebDecoderFFmpeg() : DecoderFFmpeg(nullptr){}

    unsigned long AddRef() {
        return DecoderFFmpeg::AddRef();
    }

    unsigned long Release() {
        return DecoderFFmpeg::Release();
    }
    IDecodeError initDecoder(WebStreamFFmpeg* pIStream){
        return DecoderFFmpeg::initDecoder(pIStream);
    }
    IDecodeError decodeData(WebPacketFFmpeg* pIPacket, WebFrameFFmpeg* pIFrame){
        return DecoderFFmpeg::decodeData(pIPacket,pIFrame);
    }
    void setHardware(bool needHardware){
         DecoderFFmpeg::setHardware(needHardware);
    }
    bool getHardware(){
        return DecoderFFmpeg::getHardware();
    }
};

#endif