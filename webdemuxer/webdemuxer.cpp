#include "webdemuxer.h"
#include <iostream>

using namespace emscripten;

class IODeviceWrapper : public wrapper<IODevice> {

public:
    EMSCRIPTEN_WRAPPER(IODeviceWrapper);
    int64_t ReadData(val data) {
        return call<unsigned long>("ReadData", data);
    }
    virtual int64_t SeekData(unsigned long pos, unsigned long whence) {
        return call<unsigned long>("SeekData", pos, whence);
    }
};

// Binding code
EMSCRIPTEN_BINDINGS(IODevice){
    class_<IODevice>("IODevice")
       .function("ReadData", &IODevice::ReadData, pure_virtual())
       .function("SeekData", &IODevice::SeekData, pure_virtual())
       .allow_subclass<IODeviceWrapper>("IODeviceWrapper")
       ;
}

// Binding code
EMSCRIPTEN_BINDINGS(WebDemuxerFFmpeg){
    class_<WebDemuxerFFmpeg>("WebDemuxerFFmpeg")
       .constructor<>()
       .function("initDemuxerIO", &WebDemuxerFFmpeg::initDemuxerIO, allow_raw_pointers())
       .function("initDemuxerUrl", &WebDemuxerFFmpeg::initDemuxerUrl)
       .function("demuxStream", &WebDemuxerFFmpeg::demuxStream, allow_raw_pointers())
       .function("converToNAL", &WebDemuxerFFmpeg::converToNAL, allow_raw_pointers(), allow_raw_pointers())
       .function("seek", &WebDemuxerFFmpeg::seek)
       .function("setScale", &WebDemuxerFFmpeg::setScale)
       .function("getDuration", &WebDemuxerFFmpeg::getDuration)
       .function("pause", &WebDemuxerFFmpeg::pause)
       .function("resume", &WebDemuxerFFmpeg::resume)
       .function("getStreamCount", &WebDemuxerFFmpeg::getStreamCount)
       .function("getStream", &WebDemuxerFFmpeg::getStream, allow_raw_pointers())
       .function("AddRef", &WebDemuxerFFmpeg::AddRef)
       .function("Release", &WebDemuxerFFmpeg::Release)
       ;
}

EMSCRIPTEN_BINDINGS(EnumIDemuxError) {
    enum_<IDemux::IDemuxError>("IDemuxError")
        .value("DemuxerNoError", IDemux::NoError)
        .value("DemuxerInitError", IDemux::InitError)
        .value("DemuxerParamError", IDemux::ParamError)
        .value("DemuxerDemuxerEof", IDemux::DemuxerEof)
        .value("DemuxerDemuxError", IDemux::DemuxError)
        .value("DemuxerNeedlessConver", IDemux::NeedlessConver)
        .value("DemuxerStreamIndexError", IDemux::StreamIndexError)
        .value("DemuxerNotInit", IDemux::NotInit)
        ;
}