#include "webswscaler.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(WebSwscalerFFmpeg)
{
    class_<WebSwscalerFFmpeg>("WebSwscalerFFmpeg")
        .constructor<>()
        .function("AddRef", &WebSwscalerFFmpeg::AddRef)
        .function("Release", &WebSwscalerFFmpeg::Release)
        .function("getPictureSize", &WebSwscalerFFmpeg::getPictureSize)
        .function("convertImg", &WebSwscalerFFmpeg::convertImg, allow_raw_pointers(), allow_raw_pointers())
        .function("convertImgToWebBuffer", &WebSwscalerFFmpeg::convertImgToWebBuffer, allow_raw_pointers())
        .function("uninitVideoHandle", &WebSwscalerFFmpeg::uninitVideoHandle)
        .function("converAudio", &WebSwscalerFFmpeg::converAudio, allow_raw_pointers())
        .function("uninitAudioHandle", &WebSwscalerFFmpeg::uninitAudioHandle);
}

EMSCRIPTEN_BINDINGS(IScaleFrameFormat)
{
    enum_<IScale::FrameFormat>("IScaleFrameFormat")
        .value("FormatNone", IScale::FormatNone)
        .value("FormatRGB24", IScale::FormatRGB24)
        .value("FormatRGBA", IScale::FormatRGBA)
        .value("FormatBGRA", IScale::FormatBGRA)
        .value("FormatGRAY8", IScale::FormatGRAY8)
        .value("FormatYUV420P", IScale::FormatYUV420P)
        .value("ForamtARGB", IScale::ForamtARGB)
        .value("ForamtNV12", IScale::ForamtNV12)
        .value("ForamtNV21", IScale::ForamtNV21);
}

EMSCRIPTEN_BINDINGS(IScaleError)
{
    enum_<IScale::IScaleError>("IScaleError")
        .value("NoError", IScale::NoError)
        .value("InitError", IScale::InitError)
        .value("ParamError", IScale::ParamError)
        .value("NotInit", IScale::NotInit)
        .value("ConvertError", IScale::ConvertError);
}

EMSCRIPTEN_BINDINGS(IScaleAudioSampleFormat)
{
    enum_<IScale::AudioSampleFormat>("IScaleAudioSampleFormat")
        .value("AudioFormateNone", IScale::AudioFormateNone)
        .value("AudioFormateU8", IScale::AudioFormateU8)
        .value("AudioFormateS16", IScale::AudioFormateS16)
        .value("AudioFormateS32", IScale::AudioFormateS32)
        .value("AudioFormateFLT", IScale::AudioFormateFLT)
        .value("AudioFormateDBL", IScale::AudioFormateDBL)
        .value("AudioFormateU8P", IScale::AudioFormateU8P)
        .value("AudioFormateS16P", IScale::AudioFormateS16P)
        .value("AudioFormateS32P", IScale::AudioFormateS32P)
        .value("AudioFormateFLTP", IScale::AudioFormateFLTP)
        .value("AudioFormateDBLP", IScale::AudioFormateDBLP)
        .value("AudioFormateS64", IScale::AudioFormateS64)
        .value("AudioFormateS64P", IScale::AudioFormateS64P);
}

