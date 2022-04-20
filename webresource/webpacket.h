#ifndef WEBPACKET_H
#define WEBPACKET_H

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include "../BMIMediaPlayer/playerplugins/Resource/ffmpegPacket/packetffmpeg.h"

using namespace emscripten;

class WebPacketFFmpeg : public PacketFFmpeg {
public:
    WebPacketFFmpeg() : PacketFFmpeg(nullptr){}

    unsigned long AddRef() {
        return PacketFFmpeg::AddRef();
    }

    unsigned long Release() {
        return PacketFFmpeg::Release();
    }

    void setPtsMSec(int nMSec){
        PacketFFmpeg::setPts(nMSec);
    }

    int getPtsMSec(){
        return PacketFFmpeg::getPts();
    }

    void setDtsMSec(int nMSec){
        PacketFFmpeg::setDts(nMSec);
    }

    int getDtsMSec(){
        return PacketFFmpeg::getDts();
    }

    void setData(std::string data){
        PacketFFmpeg::resetPrivateData();
        unsigned char* pData = new unsigned char[data.size()];
        memcpy(pData, data.c_str(), data.size());
        PacketFFmpeg::setPacketData(pData,data.size());
        auto releaseFunc = [&](void* privateData)
        {
            unsigned char* pData = (unsigned char*)privateData;
            delete[] pData;
        };
        PacketFFmpeg::setPrivateData(pData, releaseFunc);
    }
    val getData() {
        int packetSize = 0;
        const unsigned char *packetData = PacketFFmpeg::getPacketData(packetSize);
        return val(typed_memory_view(packetSize, packetData));
    }

    void setKeyframeFlag(bool isKeyframe){
        PacketFFmpeg::setKeyframeFlag(isKeyframe);
    }

    bool isKeyframe(){
        return PacketFFmpeg::isKeyframe();
    }

    void setStreamIndex(int streamIndex){
        PacketFFmpeg::setStreamIndex(streamIndex);
    }

    int getStreamIndex() {
        return PacketFFmpeg::getStreamIndex();
    }

    void setEofFlag(bool isEnd){
        PacketFFmpeg::setEofFlag(isEnd);
    }

    bool isEnd(){
        return PacketFFmpeg::isEnd();
    }

    std::string packetName(){
        return PacketFFmpeg::packetName();
    }

    void setPacketType(AVMediaType mediaType){
        PacketFFmpeg::setPacketType(mediaType);
    }

    AVMediaType getPacketType(){
        return PacketFFmpeg::getPacketType();
    }

    int getPacketPos(){
        return PacketFFmpeg::getPacketPos();
    }

    void setPacketPos(int packetPos){
        PacketFFmpeg::setPacketPos(packetPos);
    }

    int getPacketDuration(){
        return PacketFFmpeg::getPacketDuration();
    }

    void setPacketDuration(int packetDuration){
        PacketFFmpeg::setPacketDuration(packetDuration);
    }

    int getPacketFlag(){
        return PacketFFmpeg::getPacketFlag();
    }

    void setPacketFlag(int packetFlag){
        PacketFFmpeg::setPacketFlag(packetFlag);
    }
    
};

#endif