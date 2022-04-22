#include "decode_worker.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(DecoderData){
    class_<DecoderData>("DecoderData")
       .constructor<>()
       .function("Frame", &DecoderData::Frame, allow_raw_pointers())
       .function("Packet", &DecoderData::Packet, allow_raw_pointers())
       .function("Ret", &DecoderData::Ret)
       .function("Release", &DecoderData::Release)
       ;
}

EMSCRIPTEN_BINDINGS(DecodeWorker){
    class_<DecodeWorker>("DecodeWorker")
       .constructor<>()
       .function("initDecoder", &DecodeWorker::initDecoder, allow_raw_pointers())
       .function("Release", &DecodeWorker::Release)
       .function("decodeData", &DecodeWorker::decodeData, allow_raw_pointers(), allow_raw_pointers())
       .function("getRetData", &DecodeWorker::getRetData, allow_raw_pointers())
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
