#ifndef IPACKET_H
#define IPACKET_H

#include <unknown.h>
#include <memory>
#include <stdint.h>
#include <functional>
#include <string>

extern "C" IID IID_IPACKET;

interface IPacket : public IUNknown
{
    typedef std::function<void(void*)> ReleasePrivateCallback;

    enum AVMediaType {
        AVTypeUnknow = -1,
        AVTypeVideo,
        AVTypeAudio,
        AVTypeData,
        AVTypeSubtitle,
        AVTypeAttachment
    };

    /**
     * @brief setPrivateData    设置Packet包私有数据
     * @param privateData       私有数据指针
     * @param callback          私有数据删除函数
     */
    virtual void setPrivateData(void* privateData, ReleasePrivateCallback callback) = 0;

    /**
     * @brief resetPrivateData 重置private data
     */
    virtual void resetPrivateData() = 0;

    /**
     * @brief setPts            设置Packet的pts
     * @param nMSec             Packet的pts(毫秒)
     */
    virtual void setPts(int64_t nMSec) = 0;

    /**
     * @brief getPts            获取Packet的pts
     * @return                  Pack额头的pts(毫秒)
     */
    virtual int64_t getPts() = 0;

    /**
     * @brief setDts            设置Packet的dts
     * @param nMSec             Packet的dts(毫秒)
     */
    virtual void setDts(int64_t nMSec) = 0;

    /**
     * @brief getDts            获取Packet的dts
     * @return                  Pack额头的dts(毫秒)
     */
    virtual int64_t getDts() = 0;

    /**
     * @brief setPacketData     设置Packet的数据
     * @param packetData        Packet数据的指针
     * @param packetSize        Packet数据的大小
     */
    virtual void setPacketData(unsigned char* packetData, int packetSize) = 0;

    /**
     * @brief getPacketData     获取Packet的数据
     * @param packetSize        Packet数据的大小
     * @return                  Packet数据的指针
     */
    virtual const unsigned char* getPacketData(int& packetSize) = 0;

    /**
     * @brief setKeyframeFlag   设置关键帧标志
     * @param isKeyframe        是否是关键帧
     */
    virtual void setKeyframeFlag(bool isKeyframe) = 0;

    /**
     * @brief isKeyframe        该包是否是关键帧
     * @return                  是否是关键帧
     */
    virtual bool isKeyframe() = 0;

    /**
     * @brief setStreamIndex    设置该Packet所属的流索引
     * @param streamIndex       Packet所属的流索引
     */
    virtual void setStreamIndex(int streamIndex) = 0;

    /**
     * @brief getStreamIndex    获取Packet所属的流索引
     * @return                  Packet所属的流索引
     */
    virtual int getStreamIndex() = 0;

    /**
     * @brief setEnfFlag        设置结束标志(默认值为未结束，需要在结束时设置为TRUE)
     * @param isEnd             是否结束
     */
    virtual void setEofFlag(bool isEnd) = 0;

    /**
     * @brief isEnd             是否结束
     * @return                  结束为true，未结束为false
     */
    virtual bool isEnd() = 0;

    /**
     * @brief setPacketType     设置包的媒体类型
     * @param mediaType         媒体类型
     */
    virtual void setPacketType(AVMediaType mediaType) = 0;

    /**
     * @brief getPacketType     获取包的媒体类型
     * @return                  媒体类型
     */
    virtual AVMediaType getPacketType() = 0;

    /**
     * @brief packetName        Packet名字，用于标识Packet接口的类型
     * @return
     */
    virtual std::string packetName() = 0;

    virtual int getPacketPos() = 0;
    virtual void setPacketPos(int packetPos) = 0;
    virtual int getPacketDuration() = 0;
    virtual void setPacketDuration(int packetDuration) = 0;
    virtual int getPacketFlag() = 0;
    virtual void setPacketFlag(int packetFlag) = 0;
};

#endif // IPACKET_H
