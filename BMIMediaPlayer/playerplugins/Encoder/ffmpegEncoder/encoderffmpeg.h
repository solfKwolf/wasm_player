#ifndef ENCODERFFMPEG_H
#define ENCODERFFMPEG_H

#if defined(ENCODER_LIBRARY)
#  define ENCODERSHARED_EXPORT __declspec(dllexport)
#else
#  define ENCODERSHARED_EXPORT __declspec(dllimport)
#endif

extern "C"
{
    struct AVCodecContext;
    struct AVCodecParameters;
    struct AVPacket;
    struct AVFrame;
}

#include <unknown.h>
#include <atomic>
#include <encode.h>
#include <string>
#include <memory>
#include <frame.h>

typedef std::shared_ptr<AVCodecContext> AVCodecContextPtr;
typedef std::shared_ptr<AVPacket> AVPacketPtr;

class EncoderFFmpeg
        : public INondelegatingUnknown
        , public IEncode
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
    EncoderFFmpeg(IUNknown* pUnknowOuter);
    /**
     * @brief 析构函数
     */
    virtual ~EncoderFFmpeg();

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
    virtual IEncodeError initEncoder(IUNknown* pIStream, std::string strEncoderName = "") override;
    virtual IEncodeError encodeData(IUNknown* pIFrame, IUNknown* pIPacket) override;
    virtual void setBitrate(int64_t bitrate);
    virtual int64_t getBitrate();
    virtual void setGopSize(int gopSize) override;
    virtual int getGopSize() override;
    virtual void setFramerate(int framerate) override;
    virtual int getFramerate() override;
    virtual void setMaxQuality(int quality) override;
    virtual int getMaxQuality() override;
    virtual void setMinQuality(int quality) override;
    virtual int getMinQuality() override;

    virtual void setEncodeType(IEncodeTpye type);
    virtual IEncodeTpye getEncodeType();
private:
    void codecParFFmpegMeida2FFmpeg(AVCodecParameters *par, IUNknown* pIStream);
    std::shared_ptr<AVFrame> getFrame(IUNknown *frameUnknown);
    int convertFormat(IFrame::FrameFormat format);
private:
    AVCodecContextPtr m_pCodecContext;
    AVPacketPtr m_pPacket;
    int m_nBitrate;
    int m_nGopsize;
    int m_nFramerate;
    int m_nMaxQuality;
    int m_nMinQuality;
};

/**
 * @brief CreateInstance
 * @param pUnknowOuter 外部组件Unknown接口指针，NULL表示不聚合
 * @return
 */
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
extern "C"
#ifdef __WIN32__
ENCODERSHARED_EXPORT
#else
#endif // _WIN32
IUNknown *CreateInstance(IUNknown* pUnknowOuter = nullptr);
#endif
#endif // ENCODERFFMPEG_H
