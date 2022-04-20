#include "webframe.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(WebFrameFFmpeg){
    class_<WebFrameFFmpeg>("WebFrameFFmpeg")
       .constructor<>()
       .function("AddRef", &WebFrameFFmpeg::AddRef)
       .function("Release", &WebFrameFFmpeg::Release)
       .function("setPtsMsec", &WebFrameFFmpeg::setPtsMsec)
       .function("getPtsMsec", &WebFrameFFmpeg::getPtsMsec)
       .function("setStreamIndex", &WebFrameFFmpeg::setStreamIndex)
       .function("getStreamIndex", &WebFrameFFmpeg::getStreamIndex)
       .function("setEofFlag", &WebFrameFFmpeg::setEofFlag)
       .function("isEnd", &WebFrameFFmpeg::isEnd)
       .function("frameName", &WebFrameFFmpeg::frameName)
       .function("setFrameType", &WebFrameFFmpeg::setFrameType)
       .function("getFrameType", &WebFrameFFmpeg::getFrameType)
       .function("setFrameDuration", &WebFrameFFmpeg::setFrameDuration)
       .function("getFrameDuration", &WebFrameFFmpeg::getFrameDuration)
       .function("getFramenRate", &WebFrameFFmpeg::getFramenRate)
       .function("setFramenRate", &WebFrameFFmpeg::setFramenRate)
       .function("getPacketSize", &WebFrameFFmpeg::getPacketSize)
       .function("setPacketSize", &WebFrameFFmpeg::setPacketSize)
       .function("setData", &WebFrameFFmpeg::setData)
       .function("getData", &WebFrameFFmpeg::getData)
       // video
       .function("setFrameSize", &WebFrameFFmpeg::setFrameSize)
       .function("frameWidth", &WebFrameFFmpeg::frameWidth)
       .function("frameHeight", &WebFrameFFmpeg::frameHeight)
       .function("setFrameFormat", &WebFrameFFmpeg::setFrameFormat)
       .function("getFrameFormat", &WebFrameFFmpeg::getFrameFormat)
       // audio
       .function("setSampleRate", &WebFrameFFmpeg::setSampleRate)
       .function("getSampleRate", &WebFrameFFmpeg::getSampleRate)
       .function("setAudioChannels", &WebFrameFFmpeg::setAudioChannels)
       .function("getAduioChannels", &WebFrameFFmpeg::getAduioChannels)
       .function("setAudioSampleFormat", &WebFrameFFmpeg::setAudioSampleFormat)
       .function("getAudioSampleFormat", &WebFrameFFmpeg::getAudioSampleFormat)
       .function("getNBSamples", &WebFrameFFmpeg::getNBSamples)
       .function("setNBSamples", &WebFrameFFmpeg::setNBSamples)
       ;
}

EMSCRIPTEN_BINDINGS(FrameAVMediaType) {
    enum_<IFrame::AVMediaType>("FrameAVMediaType")
        .value("AVTypeUnknow", IFrame::AVTypeUnknow)
        .value("AVTypeVideo", IFrame::AVTypeVideo)
        .value("AVTypeAudio", IFrame::AVTypeAudio)
        .value("AVTypeData", IFrame::AVTypeData)
        .value("AVTypeSubtitle", IFrame::AVTypeSubtitle)
        .value("AVTypeAttachment", IFrame::AVTypeAttachment)
        ;
}

EMSCRIPTEN_BINDINGS(IFrameFrameFormat) {
    enum_<IFrame::FrameFormat>("IFrameFrameFormat")
        .value("FormatNone", IFrame::FormatNone)
        .value("FormatRGB24", IFrame::FormatRGB24)
        .value("FormatRGBA", IFrame::FormatRGBA)
        .value("FormatBGRA", IFrame::FormatBGRA)
        .value("FormatGRAY8", IFrame::FormatGRAY8)
        .value("FormatYUV420P", IFrame::FormatYUV420P)
        .value("ForamtARGB", IFrame::ForamtARGB)
        .value("ForamtNV12", IFrame::ForamtNV12)
        .value("ForamtNV21", IFrame::ForamtNV21)
        ;
}

EMSCRIPTEN_BINDINGS(IFrameAudioSampleFormat) {
    enum_<IFrame::AudioSampleFormat>("IFrameAudioSampleFormat")
        .value("AudioFormateNone", IFrame::AudioFormateNone)
        .value("AudioFormateU8", IFrame::AudioFormateU8)
        .value("AudioFormateS16", IFrame::AudioFormateS16)
        .value("AudioFormateS32", IFrame::AudioFormateS32)
        .value("AudioFormateFLT", IFrame::AudioFormateFLT)
        .value("AudioFormateDBL", IFrame::AudioFormateDBL)
        .value("AudioFormateU8P", IFrame::AudioFormateU8P)
        .value("AudioFormateS16P", IFrame::AudioFormateS16P)
        .value("AudioFormateS32P", IFrame::AudioFormateS32P)
        .value("AudioFormateFLTP", IFrame::AudioFormateFLTP)
        .value("AudioFormateDBLP", IFrame::AudioFormateDBLP)
        .value("AudioFormateS64", IFrame::AudioFormateS64)
        .value("AudioFormateS64P", IFrame::AudioFormateS64P)
        ;
}
