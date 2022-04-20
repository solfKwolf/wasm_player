#ifndef IFRAME_H
#define IFRAME_H

#include <unknown.h>
#include <functional>

extern "C" IID IID_IFRAME;

interface IFrame : public IUNknown
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

    enum AudioSampleFormat
    {
        AudioFormateNone = -1,
        AudioFormateU8,          ///< unsigned 8 bits
        AudioFormateS16,         ///< signed 16 bits
        AudioFormateS32,         ///< signed 32 bits
        AudioFormateFLT,         ///< float
        AudioFormateDBL,         ///< double
        AudioFormateU8P,         ///< unsigned 8 bits, planar
        AudioFormateS16P,        ///< signed 16 bits, planar
        AudioFormateS32P,        ///< signed 32 bits, planar
        AudioFormateFLTP,        ///< float, planar
        AudioFormateDBLP,        ///< double, planar
        AudioFormateS64,         ///< signed 64 bits
        AudioFormateS64P,        ///< signed 64 bits, planar
    };

    enum FrameFormat
    {
        FormatNone = -1,
        FormatRGB24 = 0,
        FormatRGBA = 1,
        FormatBGRA = 2,
        FormatGRAY8 = 3,
        FormatYUV420P = 4,
        ForamtARGB = 5,
        ForamtNV12 = 6,
        ForamtNV21 = 7,
    };

    /**
     * @brief setPrivateData    设置Frame包私有数据
     * @param privateData       私有数据指针
     * @param callback          私有数据删除函数
     */
    virtual void setPrivateData(void* privateData, ReleasePrivateCallback callback) = 0;

    /**
     * @brief resetPrivateData 重置private data
     */
    virtual void resetPrivateData() = 0;

    /**
     * @brief setPts            设置Framet的pts
     * @param nMSec             Frame的pts(毫秒)
     */
    virtual void setPts(int64_t nMSec) = 0;

    /**
     * @brief getPts            获取Frame的pts
     * @return                  Frame的pts(毫秒)
     */
    virtual int64_t getPts() = 0;

    /**
     * @brief setDuration       设置Frame的duration
     * @param duration          Frame的duration(毫秒)
     */
    virtual void setDuration(int64_t duration) = 0;

    /**
     * @brief getDuration       获取Frame的duration
     * @return                  Frame的duration(毫秒)
     */
    virtual int64_t getDuration() = 0;

    /**
     * @brief setFrameData      设置Frame的数据
     * @param frameData         Frame数据的指针
     * @param frameSize         Frame数据的大小
     */
    virtual void setFrameData(unsigned char* frameData, int frameSize) = 0;

    /**
     * @brief getFrameData      获取Frame的数据
     * @param frameSize         Frame数据的大小
     * @return                  Frame数据的指针
     */
    virtual const unsigned char* getFrameData(int& frameSize) = 0;

    /**
     * @brief setStreamIndex    设置该Frame所属的流索引
     * @param streamIndex       Frame所属的流索引
     */
    virtual void setStreamIndex(int streamIndex) = 0;

    /**
     * @brief getStreamIndex    获取Frame所属的流索引
     * @return                  Frame所属的流索引
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
     * @brief frameName        Frame名字，用于标识Frame接口的类型
     * @return
     */
    virtual std::string frameName() = 0;

    /**
     * @brief setFrameSize      设置Frame宽高（视频帧时有效）
     * @param width             Frame宽
     * @param nHeight           Frame高
     */
    virtual void setFrameSize(int width, int nHeight) = 0;

    /**
     * @brief frameWidth        获取Frame宽（视频帧时有效）
     * @return                  Frame宽
     */
    virtual int frameWidth() = 0;

    /**
     * @brief frameWidth        获取Frame高（视频帧时有效）
     * @return                  Frame高
     */
    virtual int frameHeight() = 0;

    /**
     * @brief setFrameFormat    设置视频帧图像格式（视频帧时有效）
     * @param format            图像格式
     */
    virtual void setFrameFormat(FrameFormat format) = 0;

    /**
     * @brief getFrameFormat    获取视频帧图像格式（视频帧时有效）
     * @return                  图像格式
     */
    virtual FrameFormat getFrameFormat() = 0;

    /**
     * @brief setFrameType      设置包的媒体类型
     * @param mediaType         媒体类型
     */
    virtual void setFrameType(AVMediaType mediaType) = 0;

    /**
     * @brief getFrameType      获取包的媒体类型
     * @return                  媒体类型
     */
    virtual AVMediaType getFrameType() = 0;

    /**
     * @brief setSampleRate     设置音频采样率
     * @param sampleRate
     */
    virtual void setSampleRate(int sampleRate) = 0;

    /**
     * @brief getSampleRate     获取音频采样率
     * @return
     */
    virtual int getSampleRate() = 0;

    /**
     * @brief setAudioChannels  设置音频通道数量
     * @param audioChannels
     */
    virtual void setAudioChannels(int audioChannels) = 0;

    /**
     * @brief getAduioChannels  获取音频通道数
     * @return
     */
    virtual int getAduioChannels() = 0;

    /**
     * @brief setAudioSampleFormat 设置音频数据格式
     * @param format
     */
    virtual void setAudioSampleFormat(AudioSampleFormat format) = 0;

    /**
     * @brief getAudioSampleFormat 获取音频数据格式
     * @return
     */
    virtual AudioSampleFormat getAudioSampleFormat() = 0;

    virtual int getNBSamples() = 0;
    virtual void setNBSamples(int nbSamples) = 0;

    /**
     * @brief getFrameRate 获取视频码率
     * @return
     */
    virtual int64_t getFrameRate() = 0;

    /**
     * @brief setFrameRate 设置视频码率
     * @param nFrameRate
     */
    virtual void setFrameRate(int nFrameRate) = 0;

    /**
     * @brief setPacketSize 设置Packet大小
     * @param nPacketSize
     */
    virtual void setPacketSize(int nPacketSize) = 0;

    /**
     * @brief getPacketSize 获得Packet大小
     * @return
     */
    virtual int getPacketSize() = 0;
};

#endif // IFRAME_H
