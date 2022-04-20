#ifndef WEBMUXER_H
#define WEBMUXER_H

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>

#include "../BMIMediaPlayer/playerplugins/Muxer/ffmpegMuxer/muxerffmpeg.h"
#include "../webresource/webpacket.h"
#include "../webresource/webstream.h"

using namespace emscripten;

class WebIOWriter
{
public:
    virtual ~WebIOWriter(){}  
    int64_t Write(unsigned char* data, unsigned long len) {
        return WriteData(val(typed_memory_view(len, data)));
    }

    virtual int64_t WriteData(val data) = 0;
};

class WebMuxerFFmpeg : public MuxerFFmpeg {
public:
    WebMuxerFFmpeg() : MuxerFFmpeg(nullptr){}

    unsigned long AddRef() {
        return MuxerFFmpeg::AddRef();
    }

    unsigned long Release() {
        return MuxerFFmpeg::Release();
    }

    IMuxError initMuxerUrl(std::string url, std::string format) {
        return MuxerFFmpeg::initMuxer(url, format);
    }

    IMuxError initMuxerIO(WebIOWriter *writer, std::string format) {
        if(writer == nullptr)
            return ParamError;
        auto writerIO = std::bind(&WebIOWriter::Write, writer, std::placeholders::_1, std::placeholders::_2);
        return MuxerFFmpeg::initMuxer(writerIO, format);
    } 

    IMuxError writeHeader() {
        return MuxerFFmpeg::writeHeader();
    }

    IMuxError writeTrailer() {
        return MuxerFFmpeg::writeTrailer();
    }

    IMuxError addStream(WebStreamFFmpeg* pIStream) {
        return MuxerFFmpeg::addStream(pIStream);
    }

    IMuxError muxPacket(WebPacketFFmpeg *pUNknown) {
        return MuxerFFmpeg::muxPacket(pUNknown);
    }

    void clearDemuxer() {
        MuxerFFmpeg::clearDemuxer();
    }

    std::string getStrCodecs() {
        std::vector<std::string> vecCodecString = MuxerFFmpeg::getAllCodecString();
        std::string strCodecs = "\"";
        for(int i = 0; i < vecCodecString.size(); i++) {
            strCodecs += vecCodecString.at(i);
            if (i != vecCodecString.size() -1) {
                strCodecs += ",";
            }
        }
        strCodecs += "\"";
        return strCodecs;
    }
};

#endif