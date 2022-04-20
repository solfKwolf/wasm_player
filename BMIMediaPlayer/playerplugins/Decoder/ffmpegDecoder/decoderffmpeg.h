#ifndef DECODERFFMPEG_H
#define DECODERFFMPEG_H

#if defined(DECODER_LIBRARY)
#  define DECODERSHARED_EXPORT __declspec(dllexport)
#else
#  define DECODERSHARED_EXPORT __declspec(dllimport)
#endif

extern "C"
{
    struct AVStream;
    struct AVCodec;
    struct AVCodecContext;
    struct AVCodecParameters;
}

class RealDecoder;

#include <unknown.h>
#include <decode.h>
#include <atomic>

class DecoderFFmpeg
        : public INondelegatingUnknown
        , public IDecode
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
    DecoderFFmpeg(IUNknown* pUnknowOuter);
    /**
     * @brief 析构函数
     */
    virtual ~DecoderFFmpeg();

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
    virtual IDecodeError initDecoder(IUNknown* pIStream) override;
    virtual IDecodeError decodeData(IUNknown* pIPacket, IUNknown* pIFrame) override;
    virtual void setHardware(bool needHardware) override;
    virtual bool getHardware() override;

private:
    bool initCodec(bool needHardware, AVCodecContext **ppAvCodecCtx, AVCodecParameters *codecParameter);
    void codecParFFmpegMeida2FFmpeg(AVCodecParameters *par, IUNknown* pIStream);
    int openCodec(bool needHardware, AVCodecContext **ppAvCodecCtx, IUNknown* pIStream);
private:
    RealDecoder* m_decoder;
    bool m_needHardWare;
};

/**
 * @brief CreateInstance
 * @param pUnknowOuter 外部组件Unknown接口指针，NULL表示不聚合
 * @return
 */
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
extern "C"
#ifdef __WIN32__
DECODERSHARED_EXPORT
#else
#endif // _WIN32
IUNknown *CreateInstance(IUNknown* pUnknowOuter = nullptr);
#endif

#endif // DECODERFFMPEG_H
