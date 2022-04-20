#ifndef WEBSWSCALER_H
#define WEBSWSCALER_H

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include "../BMIMediaPlayer/playerplugins/Swscale/ffmpegSwscaler/swscalerffmpeg.h"
#include "../webresource/webframe.h"

using namespace emscripten;

class WebSwscalerFFmpeg : public SwscalerFFmpeg
{

  public:
    WebSwscalerFFmpeg() : SwscalerFFmpeg(nullptr) {}

    unsigned long AddRef()
    {
        return SwscalerFFmpeg::AddRef();
    }

    unsigned long Release()
    {
        return SwscalerFFmpeg::Release();
    }

    int getPictureSize(IScale::FrameFormat format, int nWidth, int nHeight)
    {
        return SwscalerFFmpeg::getPictureSize(format, nWidth, nHeight);
    }

    IScaleError convertImgToWebBuffer(WebFrameFFmpeg *frameUnknown, int outBuffer, FrameFormat outFormat, int outWidth, int outHeight)
    {
        return SwscalerFFmpeg::convertImg(frameUnknown, (unsigned char*)outBuffer, outFormat, outWidth, outHeight);
    }
    IScaleError convertImg(WebFrameFFmpeg *frameUnknown, unsigned char *outBuffer, FrameFormat outFormat, int outWidth, int outHeight)
    {
        return SwscalerFFmpeg::convertImg(frameUnknown, outBuffer, outFormat, outWidth, outHeight);
    }
    void uninitVideoHandle()
    {
        SwscalerFFmpeg::uninitVideoHandle();
    }
    IScaleError converAudio(WebFrameFFmpeg *frameUnknown, int nAudioChannels, int nSampleRate, AudioSampleFormat sampleFormat)
    {
        return SwscalerFFmpeg::converAudio(frameUnknown, nAudioChannels, nSampleRate, sampleFormat);
    }
    void uninitAudioHandle()
    {
        SwscalerFFmpeg::uninitAudioHandle();
    }
};

#endif