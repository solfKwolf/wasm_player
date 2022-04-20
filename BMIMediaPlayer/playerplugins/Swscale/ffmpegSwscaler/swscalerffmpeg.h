#ifndef SWSCALERFFMPEG_H
#define SWSCALERFFMPEG_H

#if defined(SWSCALER_LIBRARY)
#  define SWSCALERSHARED_EXPORT __declspec(dllexport)
#else
#  define SWSCALERSHARED_EXPORT __declspec(dllimport)
#endif

extern "C"
{
    struct AVStream;
    struct AVCodec;
    struct AVCodecContext;
}

#include <unknown.h>
#include <frame.h>
#include <scale.h>
#include <atomic>
#include <memory>

extern "C"
{
    struct SwsContext;
    struct SwrContext;
    struct AVFrame;
}

class SwscalerFFmpeg
        : public INondelegatingUnknown
        , public IScale
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
    SwscalerFFmpeg(IUNknown* pUnknowOuter);
    /**
     * @brief 析构函数
     */
    virtual ~SwscalerFFmpeg();

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
    virtual int getPictureSize(FrameFormat format, int nWidth, int nHeight)override;
    virtual IScaleError convertImg(IUNknown *frameUnknown, unsigned char *outBuffer,
                                   FrameFormat outFormat, int outWidth, int outHeight)override;
    virtual void uninitVideoHandle() override;

    virtual IScaleError converAudio(IUNknown *frameUnknown, int nAudioChannels, int nSampleRate, AudioSampleFormat sampleFormat) override;
    virtual void uninitAudioHandle() override;

private:

    std::shared_ptr<AVFrame> getFrame(IUNknown *frameUnknown);

    IScaleError initVideoContext(IUNknown *frameUnknown, FrameFormat outFormat, int outWidth, int outHeight);
    int convertFormat(IScale::FrameFormat format);
    IScale::FrameFormat convertFormatFrame2Sacle(IFrame::FrameFormat format);

    int converAudioFormat(IScale::AudioSampleFormat format);
    int converAudioFormat(IFrame::AudioSampleFormat format);

    IScaleError initAudioContext(IUNknown *frameUnknown, int nAudioChannels, int nSampleRate, AudioSampleFormat sampleFormat);

    enum {MAXCHANNELS = 64};
    //setup_array函数摘自ffmpeg例程
    void setupArray(uint8_t* out[MAXCHANNELS], AVFrame* inframe, int format, int samples);
    SwsContext* m_imgConvert;
    SwrContext* m_audioConvert;

};

/**
 * @brief CreateInstance
 * @param pUnknowOuter 外部组件Unknown接口指针，NULL表示不聚合
 * @return
 */
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
extern "C"
#ifdef __WIN32__
SWSCALERSHARED_EXPORT
#else
#endif // __WIN32__
IUNknown *CreateInstance(IUNknown* pUnknowOuter = nullptr);
#endif
#endif // SWSCALERFFMPEG_H
