#ifndef WEBDEMUXER_H
#define WEBDEMUXER_H

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include "../BMIMediaPlayer/playerplugins/Demuxer/ffmpegDemuxer/demuxerffmpeg.h"
#include "../webresource/webpacket.h"
#include "../webresource/webstream.h"

using namespace emscripten;

class IODevice
{
public:
    virtual ~IODevice(){}
    int64_t Read(unsigned char* data, unsigned long len) {
        return ReadData(val(typed_memory_view(len, data)));
    }

    int64_t Seek(int64_t pos, unsigned long whence) {
        return SeekData(pos, whence);
    }


    virtual int64_t SeekData(unsigned long pos, unsigned long whence) {
        return pos + whence;
    }

    virtual int64_t ReadData(val data) = 0;
};

class WebDemuxerFFmpeg : public DemuxerFFmpeg {
public:
    WebDemuxerFFmpeg() : DemuxerFFmpeg(nullptr){}

    unsigned long AddRef() {
        return DemuxerFFmpeg::AddRef();
    }

    unsigned long Release() {
        return DemuxerFFmpeg::Release();
    }

    IDemuxError initDemuxerIO(IODevice *device) {
        if(device == nullptr)
            return ParamError;
        auto cbReadIO = std::bind(&IODevice::Read, device, std::placeholders::_1, std::placeholders::_2);
        auto cbSeekIO = std::bind(&IODevice::Seek, device, std::placeholders::_1, std::placeholders::_2);
        return initDemuxer(cbReadIO, cbSeekIO);
    }

    IDemuxError initDemuxerUrl(std::string url) {
        return initDemuxer(url);
    }

    IDemuxError demuxStream(WebPacketFFmpeg *pUNknown) {
        IDemuxError error = DemuxerFFmpeg::demuxStream(pUNknown);
        return error;
    }

    IDemuxError converToNAL(WebPacketFFmpeg *pDst, WebPacketFFmpeg *pSrc) {
        return DemuxerFFmpeg::converToNAL(pDst, pSrc);
    }

    bool seek(int nSecond) {
        return DemuxerFFmpeg::seek(nSecond);
    }

    void setScale(float scale) {
        DemuxerFFmpeg::setScale(scale);
    }

    int getDuration() {
        return DemuxerFFmpeg::duration();
    }
    void pause() {
        DemuxerFFmpeg::pause();
    }

    void resume() {
        DemuxerFFmpeg::resume();
    }

    int getStreamCount(){
        return DemuxerFFmpeg::getStreamCount();
    }

    IDemuxError getStream(int nStreamIndex, WebStreamFFmpeg* pIStream){
       return DemuxerFFmpeg::getStream(nStreamIndex,pIStream);
    }
};

#endif
