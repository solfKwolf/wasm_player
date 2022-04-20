#ifndef WEBSTREAM_H
#define WEBSTREAM_H

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include "../BMIMediaPlayer/playerplugins/Resource/ffmpegStream/streamffmpeg.h"


using namespace emscripten;

class WebStreamFFmpeg : public StreamFFmpeg {

public:
    WebStreamFFmpeg() : StreamFFmpeg(nullptr){}

    unsigned long AddRef() {
        return StreamFFmpeg::AddRef();
    }

    unsigned long Release() {
        return StreamFFmpeg::Release();
    }


    void setStreamType(AVMediaType mediaType) {
        StreamFFmpeg::setStreamType(mediaType);
    }
    AVMediaType getStreamType() {
        return StreamFFmpeg::getStreamType();
    }

    void setStreamCodecType(AVCodecType type) {
        StreamFFmpeg::setStreamCodecType(type);
    }

    AVCodecType getStreamCodecType() {
        return StreamFFmpeg::getStreamCodecType();
    }

    int getDuration() {
        return StreamFFmpeg::duration();
    }
    void setDuration(int duration) {
        StreamFFmpeg::setDuration(duration);
    }
    void setStreamIndex(int streamIndex) {
        StreamFFmpeg::setStreamIndex(streamIndex);
    }
    int getStreamIndex() {
        return StreamFFmpeg::getStreamIndex();
    }
    std::string streamName() {
        return StreamFFmpeg::streamName();
    }
    std::string getStreamTitle() {
        return StreamFFmpeg::getStreamTitle();
    }
    void setStreamTitle(std::string streamTitle) {
        StreamFFmpeg::setStreamTitle(streamTitle);
    }    

    int getFrameWidth(){
        int nWidth = 0;
        int nHeight = 0;
        StreamFFmpeg::getFrameSize(nWidth,nHeight);
        return nWidth;
    }

    int getFrameHeight(){
        int nWidth = 0;
        int nHeight = 0;
        StreamFFmpeg::getFrameSize(nWidth,nHeight);
        return nHeight;
    }

    void setFrameSize(int nWidth,int nHeight) {
        StreamFFmpeg::setFrameSize(nWidth,nHeight);
    }

    FrameFormat getFrameFormat() {
        return StreamFFmpeg::getFrameFormat();
    }

    void setFrameFormat(FrameFormat format) {
        StreamFFmpeg::setFrameFormat(format);
    }

    int getAudioSampleRate(){
        int nSampleRate = 0;
        int nChannelCount = 0;
        AudioSampleFormat nSampleFormat = AudioFormateNone;
        StreamFFmpeg::getAudioFormat(nSampleRate,nChannelCount,nSampleFormat);
        return nSampleRate;
    }

    int getAudioChannelCount(){
        int nSampleRate = 0;
        int nChannelCount = 0;
        AudioSampleFormat nSampleFormat = AudioFormateNone;
        StreamFFmpeg::getAudioFormat(nSampleRate,nChannelCount,nSampleFormat);
        return nChannelCount;
    }

    AudioSampleFormat getAudioSampleFormat(){
        int nSampleRate = 0;
        int nChannelCount = 0;
        AudioSampleFormat nSampleFormat = AudioFormateNone;
        StreamFFmpeg::getAudioFormat(nSampleRate,nChannelCount,nSampleFormat);
        return nSampleFormat;
    }

    void setAudioFormat(int nSampleRate,int nChannelCount,AudioSampleFormat nSampleFormat) {      
        StreamFFmpeg::setAudioFormat(nSampleRate,nChannelCount,nSampleFormat);
    }

    StreamRational getTimebase() {
        return  StreamFFmpeg::getTimebase();
    }
    void setTimebase(StreamRational timebase) {
        StreamFFmpeg::setTimebase(timebase);
    }
    StreamRational getFrameRate() {
         return StreamFFmpeg::getFrameRate();
    }
    void setFrameRate(StreamRational frameRate) {
        StreamFFmpeg::setFrameRate(frameRate);
    }

    val getCodecExtradataMem() {
        m_strExtradata = StreamFFmpeg::getCodecExtradata();
        return val(typed_memory_view(m_strExtradata.size(), m_strExtradata.c_str()));
    }
    void setCodecExtradata(std::string codecExtradata) {
        StreamFFmpeg::setCodecExtradata(codecExtradata);
    }

private:
    std::string m_strExtradata;
};

#endif