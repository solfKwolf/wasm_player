#ifndef WEBFRAME_H
#define WEBFRAME_H

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include <list>
#include "../BMIMediaPlayer/playerplugins/Resource/ffmpegFrame/frameffmpeg.h"

using namespace emscripten;

class WebFrameFFmpeg : public FrameFFmpeg
{
  public:
    WebFrameFFmpeg() : FrameFFmpeg(nullptr) {}

    unsigned long AddRef()
    {
        return FrameFFmpeg::AddRef();
    }

    unsigned long Release()
    {
        return FrameFFmpeg::Release();
    }

    void setPtsMsec(int nMSec)
    {
        FrameFFmpeg::setPts(nMSec);
    }
    int getPtsMsec()
    {
        return FrameFFmpeg::getPts();
    }
    void setStreamIndex(int streamIndex)
    {
        FrameFFmpeg::setStreamIndex(streamIndex);
    }
    int getStreamIndex()
    {
        return FrameFFmpeg::getStreamIndex();
    }
    void setEofFlag(bool isEnd)
    {
        FrameFFmpeg::setEofFlag(isEnd);
    }
    bool isEnd()
    {
        return FrameFFmpeg::isEnd();
    }
    std::string frameName()
    {
        return FrameFFmpeg::frameName();
    }
    void setFrameType(AVMediaType mediaType)
    {
        FrameFFmpeg::setFrameType(mediaType);
    }
    AVMediaType getFrameType()
    {
        return FrameFFmpeg::getFrameType();
    }
    void setFrameDuration(int duration)
    {
        FrameFFmpeg::setDuration(duration);
    }
    int getFrameDuration()
    {
        return FrameFFmpeg::getDuration();
    }
    int getFramenRate()
    {
        return FrameFFmpeg::getFrameRate();
    }
    void setFramenRate(int nFrameRate)
    {
        FrameFFmpeg::setFrameRate(nFrameRate);
    }
    int getPacketSize()
    {
        return FrameFFmpeg::getPacketSize();
    }
    void setPacketSize(int nPacketSize)
    {
        FrameFFmpeg::setPacketSize(nPacketSize);
    }

    void setData(std::string strFrameData)
    {
        FrameFFmpeg::resetPrivateData();
        unsigned char *pData = new unsigned char[strFrameData.size()];
        memcpy(pData, strFrameData.c_str(), strFrameData.size());
        FrameFFmpeg::setFrameData(pData, strFrameData.size());
        auto releaseFunc = [&](void *privateData) {
            unsigned char *pData = (unsigned char *)privateData;
            delete[] pData;
        };
        FrameFFmpeg::setPrivateData(pData, releaseFunc);
    }
    val getData()
    {
        int frameSize = 0;
        const unsigned char *frameData = FrameFFmpeg::getFrameData(frameSize);
        return val(typed_memory_view(frameSize, frameData));
    }

    // video
    void setFrameSize(int width, int nHeight)
    {
        FrameFFmpeg::setFrameSize(width, nHeight);
    }
    int frameWidth()
    {
        return FrameFFmpeg::frameWidth();
    }
    int frameHeight()
    {
        return FrameFFmpeg::frameHeight();
    }
    void setFrameFormat(FrameFormat format)
    {
        FrameFFmpeg::setFrameFormat(format);
    }
    FrameFormat getFrameFormat()
    {
        return FrameFFmpeg::getFrameFormat();
    }

    // audio
    void setSampleRate(int sampleRate)
    {
        FrameFFmpeg::setSampleRate(sampleRate);
    }
    int getSampleRate()
    {
        return FrameFFmpeg::getSampleRate();
    }
    void setAudioChannels(int audioChannels)
    {
        FrameFFmpeg::setAudioChannels(audioChannels);
    }
    int getAduioChannels()
    {
        return FrameFFmpeg::getAduioChannels();
    }
    void setAudioSampleFormat(AudioSampleFormat format)
    {
        FrameFFmpeg::setAudioSampleFormat(format);
    }
    AudioSampleFormat getAudioSampleFormat()
    {
        return FrameFFmpeg::getAudioSampleFormat();
    }
    int getNBSamples()
    {
        return FrameFFmpeg::getNBSamples();
    }
    void setNBSamples(int nbSamples)
    {
        FrameFFmpeg::setNBSamples(nbSamples);
    }
};


#endif