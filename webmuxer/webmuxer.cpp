#include "webmuxer.h"
#include <iostream>

using namespace emscripten;

class WebIOWriterWrapper : public wrapper<WebIOWriter> {
public:
    EMSCRIPTEN_WRAPPER(WebIOWriterWrapper);
    int64_t WriteData(val data) {
        return call<unsigned long>("WriteData", data);
    }
};

// Binding code
EMSCRIPTEN_BINDINGS(WebIOWriter){
    class_<WebIOWriter>("WebIOWriter")
    .function("WriteData", &WebIOWriter::WriteData, pure_virtual())
    .allow_subclass<WebIOWriterWrapper>("WebIOWriterWrapper")
    ;
}

// Binding code
EMSCRIPTEN_BINDINGS(WebMuxerFFmpeg){
    class_<WebMuxerFFmpeg>("WebMuxerFFmpeg")
    .constructor<>()
    .function("initMuxerUrl", &WebMuxerFFmpeg::initMuxerUrl)
    .function("initMuxerIO", &WebMuxerFFmpeg::initMuxerIO, allow_raw_pointers())
    .function("writeHeader", &WebMuxerFFmpeg::writeHeader)
    .function("writeTrailer", &WebMuxerFFmpeg::writeTrailer)
    .function("addStream", &WebMuxerFFmpeg::addStream ,allow_raw_pointers())
    .function("muxPacket", &WebMuxerFFmpeg::muxPacket, allow_raw_pointers())
    .function("clearDemuxer", &WebMuxerFFmpeg::clearDemuxer)
    .function("getStrCodecs", &WebMuxerFFmpeg::getStrCodecs)
    .function("AddRef", &WebMuxerFFmpeg::AddRef)
    .function("Release", &WebMuxerFFmpeg::Release)
    ;
}

EMSCRIPTEN_BINDINGS(EnumIMuxError) {
    enum_<IMux::IMuxError>("IMuxError")
        .value("NoError", IMux::NoError)
        .value("ParamError", IMux::ParamError)
        .value("InitError", IMux::InitError)
        .value("OpenURLError", IMux::OpenURLError)
        .value("CreateStreamError", IMux::CreateStreamError)
        .value("MuxError", IMux::MuxError)
        ;
}