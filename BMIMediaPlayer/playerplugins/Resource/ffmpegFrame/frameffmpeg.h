#ifndef FRAMEFFMPEG_H
#define FRAMEFFMPEG_H

#if defined(FRAME_LIBRARY)
#  define FRAMESHARED_EXPORT __declspec(dllexport)
#else
#  define FRAMESHARED_EXPORT __declspec(dllimport)
#endif

#include <unknown.h>
#include <frame.h>
#include <atomic>
#include <vector>
#include <memory>

class FrameFFmpeg
        : public INondelegatingUnknown
        , public IFrame
{
    struct FrameData {
        unsigned char* data;
        int size;
    };

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
    FrameFFmpeg(IUNknown* pUnknowOuter);
    /**
     * @brief 析构函数
     */
    virtual ~FrameFFmpeg();

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
    virtual void setPrivateData(void* privateData, ReleasePrivateCallback callback) override;
    virtual void resetPrivateData() override;
    virtual void setPts(int64_t nMSec) override;
    virtual int64_t getPts() override;
    virtual void setStreamIndex(int streamIndex) override;
    virtual int getStreamIndex() override;
    virtual void setEofFlag(bool isEnd) override;
    virtual bool isEnd() override;
    virtual std::string frameName() override;
    virtual void setFrameType(AVMediaType mediaType) override;
    virtual AVMediaType getFrameType() override;
    virtual void setDuration(int64_t duration) override;
    virtual int64_t getDuration() override;
    virtual int64_t getFrameRate() override;
    virtual void setFrameRate(int nFrameRate)override;
    virtual int getPacketSize() override;
    virtual void setPacketSize(int nPacketSize)override;

    virtual void setFrameData(unsigned char* frameData, int frameSize) override;
    virtual const unsigned char* getFrameData(int& frameSize) override;

    // video
    virtual void setFrameSize(int width, int nHeight) override;
    virtual int frameWidth() override;
    virtual int frameHeight() override;
    virtual void setFrameFormat(FrameFormat format) override;
    virtual FrameFormat getFrameFormat() override;

    // audio
    virtual void setSampleRate(int sampleRate) override;
    virtual int getSampleRate() override;
    virtual void setAudioChannels(int audioChannels) override;
    virtual int getAduioChannels() override;
    virtual void setAudioSampleFormat(AudioSampleFormat format) override;
    virtual AudioSampleFormat getAudioSampleFormat() override;
    virtual int getNBSamples() override;
    virtual void setNBSamples(int nbSamples) override;

private:
    std::shared_ptr<void> m_pPrivateData;
    int64_t m_pts;
    int64_t m_duration;
    bool m_isKeyframe;
    int m_nStreamIndex;
    bool m_isEnd;
    int m_nWidth;
    int m_nHeight;
    FrameFormat m_format;
    AVMediaType m_mediaType;

    FrameData m_frameData;
    int m_nCurrentDataIndex;

    int m_nSampleRate;
    int m_nAudioChannels;
    AudioSampleFormat m_audioSampleFormat;
    int m_nNBSamples;

    int64_t m_nFrameRate;
    int m_nPacketSize;
};

/**
 * @brief CreateInstance
 * @param pUnknowOuter 外部组件Unknown接口指针，NULL表示不聚合
 * @return
 */
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
extern "C"
#ifdef __WIN32__
FRAMESHARED_EXPORT
#else
#endif // __WIN32__
IUNknown *CreateInstance(IUNknown* pUnknowOuter = nullptr);
#endif
#endif // FRAMEFFMPEG_H
