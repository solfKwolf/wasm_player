#include "webdecoder.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(WebDecoderFFmpeg){
    class_<WebDecoderFFmpeg>("WebDecoderFFmpeg")
       .constructor<>()
       .function("initDecoder", &WebDecoderFFmpeg::initDecoder, allow_raw_pointers())
       .function("decodeData", &WebDecoderFFmpeg::decodeData, allow_raw_pointers(), allow_raw_pointers())
       .function("setHardware",&WebDecoderFFmpeg::setHardware)
       .function("getHardware",&WebDecoderFFmpeg::getHardware)
       .function("AddRef", &WebDecoderFFmpeg::AddRef)
       .function("Release", &WebDecoderFFmpeg::Release)
       ;
}

EMSCRIPTEN_BINDINGS(IDecodeError) {
    enum_<IDecode::IDecodeError>("IDecodeError")
        .value("NoError", IDecode::NoError)
        .value("InitError", IDecode::InitError)
        .value("ParamError", IDecode::ParamError)
        .value("CodecOpenError", IDecode::CodecOpenError)
        .value("DecodeEof", IDecode::DecodeEof)
        .value("DecodeError", IDecode::DecodeError)
        .value("FrameUseless", IDecode::FrameUseless)
        .value("DecoderInvalid", IDecode::DecoderInvalid)
        ;
}
