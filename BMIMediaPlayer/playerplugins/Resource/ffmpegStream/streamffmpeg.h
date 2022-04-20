#ifndef STREAMFFMPEG_H
#define STREAMFFMPEG_H

#if defined(STREAM_LIBRARY)
#  define STREAMSHARED_EXPORT __declspec(dllexport)
#else
#  define STREAMSHARED_EXPORT __declspec(dllimport)
#endif

#include <unknown.h>
#include <stream.h>
#include <atomic>

class StreamFFmpeg
        : public INondelegatingUnknown
        , public IAVStream
{
public:
    // IUNknown
    virtual HRESULT __stdcall QueryInterface(const IID& iid, void **ppv) override;
    virtual ULONG __stdcall AddRef() override;
    virtual ULONG __stdcall Release() override;

    // INondelegatingUnknown
    virtual HRESULT __stdcall NondelegatingQueryInterface(const IID& iid, void **ppv) override;
    virtual ULONG __stdcall NondelegatingAddRef() override;
    virtual ULONG __stdcall NondelegatingRelease() override;
    /**
     * @brief 构造函数
     * @param pUnknowOuter 外部组件Unknown接口指针，nullptr表示不聚合
     */
    StreamFFmpeg(IUNknown* pUnknowOuter);
    /**
     * @brief 析构函数
     */
    virtual ~StreamFFmpeg();

private:
    /**
     * @brief 组件实例的引用计数
     */
    std::atomic_ulong m_cRef;

    /**
     * @brief 指向非代理Unknown接口或外部组件Unknown接口
     */
    IUNknown* m_pUnknowOuter;

public:
    virtual void setStreamType(AVMediaType mediaType) override;
    virtual AVMediaType getStreamType() override;
    virtual void setStreamCodecType(AVCodecType type) override;
    virtual AVCodecType getStreamCodecType() override;
    virtual int64_t duration() override;
    virtual void setDuration(int64_t duration) override;
    virtual void setStreamIndex(int streamIndex) override;
    virtual int getStreamIndex() override;
    virtual void setPrivateData(void* privateData, ReleasePrivateCallback callback) override;
    virtual std::string streamName() override;
    virtual std::string getStreamTitle() override;
    virtual void setStreamTitle(std::string streamTitle) override;
    virtual void getFrameSize(int &nWidth, int &nHeight) override;
    virtual void setFrameSize(int nWidth, int nHeight) override;
    virtual FrameFormat getFrameFormat() override;
    virtual void setFrameFormat(FrameFormat format) override;
    virtual void getAudioFormat(int &nSampleRate, int &nChannelCount, AudioSampleFormat &nSampleFormat) override;
    virtual void setAudioFormat(int nSampleRate, int nChannelCount, AudioSampleFormat nSampleFormat) override;
    virtual StreamRational getTimebase() override;
    virtual void setTimebase(StreamRational timebase) override;
    virtual StreamRational getFrameRate() override;
    virtual void setFrameRate(StreamRational frameRate) override;

    virtual int64_t getBitrate() override;
    virtual void setBitrate(int64_t bitrate) override;
    virtual std::string getCodecExtradata() override;
    virtual void setCodecExtradata(std::string codecExtradata) override;

private:
    AVMediaType m_mediaType;
    AVCodecType m_codecType;
    int64_t m_duration;
    int m_streamIndex;
    std::shared_ptr<void> m_pPrivateData;

    std::string m_streamTitle;
    int m_nWidth;
    int m_nHeight;
    int m_nSampleRate;
    int m_nChannelCount;
    FrameFormat m_videoFrameFormat;
    AudioSampleFormat m_nSampleFormat;
    StreamRational m_timebase;
    StreamRational m_framerate;
    int64_t m_bitrate;
    std::string m_strCodecExtradata;
};

/**
 * @brief CreateInstance
 * @param pUnknowOuter 外部组件Unknown接口指针，NULL表示不聚合
 * @return
 */
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
extern "C"
#ifdef __WIN32__
STREAMSHARED_EXPORT
#else
#endif // _WIN32__
IUNknown *CreateInstance(IUNknown* pUnknowOuter = nullptr);
#endif
#endif // STREAMFFMPEG_H
