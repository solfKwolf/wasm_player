#include "webpacket.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(WebPacketFFmpeg){
    class_<WebPacketFFmpeg>("WebPacketFFmpeg")
       .constructor<>()
       .function("AddRef", &WebPacketFFmpeg::AddRef)
       .function("Release", &WebPacketFFmpeg::Release)
       .function("setPtsMSec", &WebPacketFFmpeg::setPtsMSec)
       .function("getPtsMSec", &WebPacketFFmpeg::getPtsMSec)
       .function("setDtsMSec", &WebPacketFFmpeg::setDtsMSec)
       .function("getDtsMSec", &WebPacketFFmpeg::getDtsMSec)
       .function("setKeyframeFlag", &WebPacketFFmpeg::setKeyframeFlag)
       .function("isKeyframe", &WebPacketFFmpeg::isKeyframe)
       .function("setEofFlag", &WebPacketFFmpeg::setEofFlag)
       .function("isEnd", &WebPacketFFmpeg::isEnd)
       .function("packetName", &WebPacketFFmpeg::packetName)
       .function("getPacketFlag", &WebPacketFFmpeg::getPacketFlag)
       .function("setPacketFlag", &WebPacketFFmpeg::setPacketFlag)
       .function("getPacketPos", &WebPacketFFmpeg::getPacketPos)
       .function("setPacketPos", &WebPacketFFmpeg::setPacketPos)
       .function("getPacketDuration", &WebPacketFFmpeg::getPacketDuration)
       .function("setPacketDuration", &WebPacketFFmpeg::setPacketDuration)
       .function("setPacketType", &WebPacketFFmpeg::setPacketType)
       .function("getPacketType", &WebPacketFFmpeg::getPacketType)
       .function("getStreamIndex", &WebPacketFFmpeg::getStreamIndex)
       .function("getData", &WebPacketFFmpeg::getData)
       .function("setStreamIndex",&WebPacketFFmpeg::setStreamIndex)
       .function("setData",&WebPacketFFmpeg::setData)

       ;
}

EMSCRIPTEN_BINDINGS(PacketAVMediaType) {
    enum_<IPacket::AVMediaType>("PacketAVMediaType")
        .value("AVTypeUnknow", IPacket::AVTypeUnknow)
        .value("AVTypeVideo", IPacket::AVTypeVideo)
        .value("AVTypeAudio", IPacket::AVTypeAudio)
        .value("AVTypeData", IPacket::AVTypeData)
        .value("AVTypeSubtitle", IPacket::AVTypeSubtitle)
        .value("AVTypeAttachment", IPacket::AVTypeAttachment)
        ;
}