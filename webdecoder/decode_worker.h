#ifndef DECODE_WORKER_H
#define DECODE_WORKER_H

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include "../BMIMediaPlayer/playerplugins/Decoder/ffmpegDecoder/decoderffmpeg.h"
#include "../webresource/webpacket.h"
#include "../webresource/webstream.h"
#include "../webresource/webframe.h"
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <iostream>

struct DecoderData {
    WebPacketFFmpeg* pIPacket;
    WebFrameFFmpeg* pIFrame;
    IDecode::IDecodeError ret;

    DecoderData(WebPacketFFmpeg* packet, WebFrameFFmpeg* frame) {
        pIPacket = packet;
        pIFrame = frame;
    }
    DecoderData(){}

    static DecoderData* RetData(DecoderData d) {
        DecoderData* retData = new DecoderData;
        retData->pIPacket = d.pIPacket;
        retData->pIFrame = d.pIFrame;
        retData->ret = d.ret;
        return retData;
    }

    WebFrameFFmpeg* Frame() {
        return pIFrame;
    }

    WebPacketFFmpeg* Packet() {
        return pIPacket;
    }

    IDecode::IDecodeError Ret() {
        return ret;
    }

    void Release() {
        delete this;
    }
};

class DecodeWorker {
private:
    DecoderFFmpeg* decoder;
    std::thread runner;
    bool isRunning;
    std::list<DecoderData> datalist;
    std::mutex mutex;
    std::condition_variable cond;

    std::list<DecoderData> retDatalist;
public:
    DecodeWorker() {
        decoder = nullptr;
        isRunning = false;
    }
    ~DecodeWorker() {
        isRunning = false;
        cond.notify_one();
        if (runner.joinable()) {
            runner.join();
        }
        if (decoder != nullptr) {
            decoder->Release();
            decoder = nullptr;
        }
    }
    IDecode::IDecodeError initDecoder(WebStreamFFmpeg* pIStream){
        decoder = new DecoderFFmpeg(nullptr);
        decoder->AddRef();
        IDecode::IDecodeError ret = decoder->initDecoder(pIStream);
        if (ret != IDecode::NoError) {
            return ret;
        }

        runner = std::thread(std::bind(&DecodeWorker::decodeProccess, this));
        return ret;
    }

    void Release() {
        delete this;
    }

    void getRetData(DecoderData *retData) {
        std::unique_lock<std::mutex> lck(mutex);
         if (retDatalist.empty()) {
            retData->ret = IDecode::ParamError;
            return;
        }
        DecoderData data = retDatalist.front();
        retDatalist.pop_front();
        retData->pIFrame = data.pIFrame;
        retData->pIPacket = data.pIPacket;
        retData->ret = data.ret;
    }

    void decodeData(WebPacketFFmpeg* pIPacket, WebFrameFFmpeg* pIFrame) {
        std::unique_lock<std::mutex> lck(mutex);
        pIPacket->AddRef();
        pIFrame->AddRef();
        
        datalist.push_back(DecoderData(pIPacket, pIFrame));
        cond.notify_one();
    }

    DecoderData getdata() {
        std::unique_lock<std::mutex> lck(mutex);
        if (datalist.empty()) {
            cond.wait(lck); // 存在数据，继续处理
        }
        if (datalist.empty()) {
            return DecoderData(nullptr, nullptr);
        }
        DecoderData data = datalist.front();
        datalist.pop_front();
        return data;
    }

    void pushRetData(DecoderData data) {
        std::unique_lock<std::mutex> lck(mutex);
        retDatalist.push_back(data);
    }

    void decodeProccess() {
        isRunning = true;
        while (isRunning) {
            DecoderData data = getdata();
            if (data.pIFrame == nullptr && data.pIPacket == nullptr) {
                continue;
            }
            IDecode::IDecodeError ret = decoder->decodeData(data.pIPacket, data.pIFrame);
            data.ret = ret;
            pushRetData(data);
        }
    }
};

#endif