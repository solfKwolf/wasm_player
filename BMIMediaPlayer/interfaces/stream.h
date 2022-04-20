#ifndef IAVSTREAM_H
#define IAVSTREAM_H

#include <unknown.h>
#include <memory>
#include <functional>
#include <string>

extern "C" IID IID_ISTREAM;

interface IAVStream : public IUNknown
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

    enum AVCodecType {
        CodecTypeUnkow = -1,
        CodecTypeH264,
        CodecTypeHevc,
        CodecTypeAAC,
        CodecTypePCMMULAM,
        CodecTypePCMALAW
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

    struct StreamRational{
        int num; ///< Numerator
        int den; ///< Denominator
    };

    /**
     * @brief setStreamType     设置流的媒体类型
     * @param mediaType         媒体类型
     */
    virtual void setStreamType(AVMediaType mediaType) = 0;

    /**
     * @brief getStreamType     获取流的媒体类型
     * @return                  媒体类型
     */
    virtual AVMediaType getStreamType() = 0;

    /**
     * @brief setStreamCodecType    设置流对应解码器类型
     * @param type                  解码器类型
     */
    virtual void setStreamCodecType(AVCodecType type) = 0;

    /**
     * @brief getStreamCodecType    获取流对应解码器类型
     * @return
     */
    virtual AVCodecType getStreamCodecType() = 0;

    /**
     * @brief duration          流长度
     * @return                  流长度(秒)
     */
    virtual int64_t duration() = 0;

    /**
     * @brief setDuration       设置流长度
     * @param duration          流长度(秒)
     */
    virtual void setDuration(int64_t duration) = 0;

    /**
     * @brief setStreamIndex    设置流索引
     * @param streamIndex       流索引
     */
    virtual void setStreamIndex(int streamIndex) = 0;

    /**
     * @brief getStreamIndex    获取流索引
     * @return                  流索引
     */
    virtual int getStreamIndex() = 0;

    /**
     * @brief setPrivateData    设置流私有数据
     * @param privateData       私有数据指针
     * @param callback          私有数据释放函数
     */
    virtual void setPrivateData(void* privateData, ReleasePrivateCallback callback) = 0;

    /**
     * @brief streamName        stream名称，用于标识Stream接口类型
     * @return
     */
    virtual std::string streamName() = 0;

    /**
     * @brief getStreamTitle        设置流标题
     * @return
     */
    virtual std::string getStreamTitle() = 0;

    /**
     * @brief getStreamTitle        获取流标题
     * @return
     */
    virtual void setStreamTitle(std::string strTitle) = 0;

    /**
     * @brief getFrameSize          获取流解码出的帧大小
     * @param nWidth                传出参数，帧宽度
     * @param nHeight               传出参数，帧高度
     */
    virtual void getFrameSize(int &nWidth, int &nHeight) = 0;

    /**
     * @brief setFrameSize          设置流解码出的帧大小
     * @param nWidth                设置参数，帧宽度
     * @param nHeight               设置参数，帧高度
     */
    virtual void setFrameSize(int nWidth, int nHeight) = 0;

    /**
     * @brief getFrameFormat        获取视频帧格式
     * @return                      视频帧格式
     */
    virtual FrameFormat getFrameFormat() = 0;

    /**
     * @brief setFrameFormat        设置视频帧格式
     * @param format                视频帧格式
     */
    virtual void setFrameFormat(FrameFormat format) = 0;

    /**
     * @brief getAudioFormat        获取流解码出的音频信息
     * @param nSampleRate           传出参数，音频采样率
     * @param nChannelCount         传出参数，音频通道数
     * @param nSampleFormat         传出参数，音频格式
     */
    virtual void getAudioFormat(int &nSampleRate, int &nChannelCount, AudioSampleFormat &nSampleFormat) = 0;

    /**
     * @brief setAudioFormat        设置流解码出的音频信息
     * @param nSampleRate           设置参数，音频采样率
     * @param nChannelCount         设置参数，音频通道数
     * @param nSampleFormat         设置参数，音频格式
     */
    virtual void setAudioFormat(int nSampleRate, int nChannelCount, AudioSampleFormat nSampleFormat) = 0;

    /**
     * @brief getTimebase           获取流的时间基准
     * @return                      返回时间基准
     */
    virtual StreamRational getTimebase() = 0;

    /**
     * @brief setTimebase           设置流的时间基准
     * @param timebase              流的时间基准
     */
    virtual void setTimebase(StreamRational timebase) = 0;

    /**
     * @brief getFrameRate          获取流帧率
     * @return
     */
    virtual StreamRational getFrameRate() = 0;

    /**
     * @brief setFrameSize          设置流帧率
     * @param frameRate
     */
    virtual void setFrameRate(StreamRational frameRate) = 0;

    /**
     * @brief getBitrate            获取流比特率
     * @return                      流比特率
     */
    virtual int64_t getBitrate() = 0;

    /**
     * @brief setBitrate            设置比特率
     * @param bitrate               比特率
     */
    virtual void setBitrate(int64_t bitrate) = 0;

    virtual std::string getCodecExtradata() = 0;
    virtual void setCodecExtradata(std::string codecExtradata) = 0;
};

#endif // IAVSTREAM_H
