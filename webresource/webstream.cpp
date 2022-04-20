#include "webstream.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(WebStreamFFmpeg){
    class_<WebStreamFFmpeg>("WebStreamFFmpeg")
       .constructor<>()
       .function("AddRef", &WebStreamFFmpeg::AddRef)
       .function("Release", &WebStreamFFmpeg::Release)
       .function("setStreamType", &WebStreamFFmpeg::setStreamType)
       .function("getStreamType", &WebStreamFFmpeg::getStreamType)
       .function("setStreamCodecType", &WebStreamFFmpeg::setStreamCodecType)
       .function("getStreamCodecType", &WebStreamFFmpeg::getStreamCodecType)
       .function("getDuration", &WebStreamFFmpeg::getDuration)
       .function("setDuration", &WebStreamFFmpeg::setDuration)
       .function("setStreamIndex", &WebStreamFFmpeg::setStreamIndex)
       .function("getStreamIndex", &WebStreamFFmpeg::getStreamIndex)
       .function("streamName", &WebStreamFFmpeg::streamName)
       .function("getStreamTitle", &WebStreamFFmpeg::getStreamTitle)
       .function("setStreamTitle", &WebStreamFFmpeg::setStreamTitle)
       .function("getFrameWidth", &WebStreamFFmpeg::getFrameWidth)
       .function("getFrameHeight", &WebStreamFFmpeg::getFrameHeight)
       .function("setFrameSize", &WebStreamFFmpeg::setFrameSize)
       .function("getFrameFormat", &WebStreamFFmpeg::getFrameFormat)
       .function("setFrameFormat", &WebStreamFFmpeg::setFrameFormat)
       .function("getAudioSampleRate", &WebStreamFFmpeg::getAudioSampleRate)
       .function("getAudioChannelCount", &WebStreamFFmpeg::getAudioChannelCount)
       .function("getAudioSampleFormat", &WebStreamFFmpeg::getAudioSampleFormat)
       .function("setAudioFormat", &WebStreamFFmpeg::setAudioFormat)
       .function("getTimebase", &WebStreamFFmpeg::getTimebase)
       .function("setTimebase", &WebStreamFFmpeg::setTimebase)
       .function("getFrameRate", &WebStreamFFmpeg::getFrameRate)
       .function("setFrameRate", &WebStreamFFmpeg::setFrameRate)
       .function("getCodecExtradataMem", &WebStreamFFmpeg::getCodecExtradataMem)
       .function("setCodecExtradata", &WebStreamFFmpeg::setCodecExtradata)

       ;
}

EMSCRIPTEN_BINDINGS(StreamAVMediaType) {
    enum_<IAVStream::AVMediaType>("StreamAVMediaType")
        .value("AVTypeUnknow", IAVStream::AVTypeUnknow)
        .value("AVTypeVideo", IAVStream::AVTypeVideo)
        .value("AVTypeAudio", IAVStream::AVTypeAudio)
        .value("AVTypeData", IAVStream::AVTypeData)
        .value("AVTypeSubtitle", IAVStream::AVTypeSubtitle)
        .value("AVTypeAttachment", IAVStream::AVTypeAttachment)
        ;
}

EMSCRIPTEN_BINDINGS(StreamAudioSampleFormat) {
    enum_<IAVStream::AudioSampleFormat>("StreamAudioSampleFormat")
        .value("AudioFormateNone", IAVStream::AudioFormateNone)
        .value("AudioFormateU8", IAVStream::AudioFormateU8)
        .value("AudioFormateS16", IAVStream::AudioFormateS16)
        .value("AudioFormateS32", IAVStream::AudioFormateS32)
        .value("AudioFormateFLT", IAVStream::AudioFormateFLT)
        .value("AudioFormateDBL", IAVStream::AudioFormateDBL)
        .value("AudioFormateU8P", IAVStream::AudioFormateU8P)
        .value("AudioFormateS16P", IAVStream::AudioFormateS16P)
        .value("AudioFormateS32P", IAVStream::AudioFormateS32P)
        .value("AudioFormateFLTP", IAVStream::AudioFormateFLTP)
        .value("AudioFormateDBLP", IAVStream::AudioFormateDBLP)
        .value("AudioFormateS64", IAVStream::AudioFormateS64)
        .value("AudioFormateS64P", IAVStream::AudioFormateS64P)
        ;
}

EMSCRIPTEN_BINDINGS(StreamAVCodecType) {
    enum_<IAVStream::AVCodecType>("StreamAVCodecType")
        .value("CodecTypeUnkow", IAVStream::CodecTypeUnkow)
        .value("CodecTypeH264", IAVStream::CodecTypeH264)
        .value("CodecTypeHevc", IAVStream::CodecTypeHevc)
        .value("CodecTypeAAC", IAVStream::CodecTypeAAC)
        .value("CodecTypePCMMULAM", IAVStream::CodecTypePCMMULAM)
        .value("CodecTypePCMALAW", IAVStream::CodecTypePCMALAW)
        ;
}

EMSCRIPTEN_BINDINGS(StreamFrameFormat) {
    enum_<IAVStream::FrameFormat>("StreamFrameFormat")
        .value("FormatNone", IAVStream::FormatNone)
        .value("FormatRGB24", IAVStream::FormatRGB24)
        .value("FormatRGBA", IAVStream::FormatRGBA)
        .value("FormatBGRA", IAVStream::FormatBGRA)
        .value("FormatGRAY8", IAVStream::FormatGRAY8)
        .value("FormatYUV420P", IAVStream::FormatYUV420P)
        .value("ForamtARGB", IAVStream::ForamtARGB)
        .value("ForamtNV12", IAVStream::ForamtNV12)
        .value("ForamtNV21", IAVStream::ForamtNV21)
        ;
}

EMSCRIPTEN_BINDINGS(StreamRational){
    value_array<IAVStream::StreamRational>("StreamRational")
       .element(&IAVStream::StreamRational::num)
       .element(&IAVStream::StreamRational::den)
       ;
}