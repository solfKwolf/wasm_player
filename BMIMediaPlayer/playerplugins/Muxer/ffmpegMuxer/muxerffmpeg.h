#ifndef MUXERFFMPEG_H
#define MUXERFFMPEG_H

#if defined(MUXER_LIBRARY)
#  define MUXERSHARED_EXPORT __declspec(dllexport)
#else
#  define MUXERSHARED_EXPORT __declspec(dllimport)
#endif

extern "C"
{
    struct AVFormatContext;
    struct AVCodecParameters;
    struct AVPacket;
    struct AVIOContext;
}

#include <unknown.h>
#include <mux.h>
#include <atomic>
#include <stream.h>

typedef std::shared_ptr<AVFormatContext> AVFormatContextPtr;
typedef std::shared_ptr<AVIOContext> AVIOContextPtr;

class MuxerFFmpeg
        : public INondelegatingUnknown
        , public IMux
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
    MuxerFFmpeg(IUNknown* pUnknowOuter);
    /**
     * @brief 析构函数
     */
    virtual ~MuxerFFmpeg();

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
    virtual IMuxError initMuxer(std::string url, std::string format) override;
    virtual IMuxError initMuxer(IOCallBack outputCallback, std::string format) override;
    virtual IMuxError writeHeader() override;
    virtual IMuxError writeTrailer() override;
    virtual IMuxError addStream(IUNknown *pIStream) override;
    virtual IMuxError muxPacket(IUNknown *pUnknow) override;
    std::vector<std::string> getAllCodecString() override;

    static int writePack(void *pUser, uint8_t *buf, int bufSize);
    void clearDemuxer();

private:
    void codecParFFmpegMeida2FFmpeg(AVCodecParameters *par, IUNknown *pIStream);
    void getPacket(AVPacket* pkt, IUNknown* pIPacket);
    int converAudioFormat(IAVStream::AudioSampleFormat format);

    void parseAACHead(const char pData[2], int &channels, int &samplerate);
    unsigned int getSrIndex(unsigned int samples);
    void makeDsi(unsigned char samplingFrequencyIndex, unsigned char channels, unsigned char* dsi);
    std::string getCodecString(AVCodecParameters *par);

private:
    AVFormatContextPtr m_outputContext;

    IOCallBack m_outputCallback;
    AVIOContextPtr m_ioContext;
    std::shared_ptr<unsigned char> m_writeBuffer;

    enum {
        BUF_SIZE = 1024 * 1024 * 4,
    };

private:
};

/**
 * @brief CreateInstance
 * @param pUnknowOuter 外部组件Unknown接口指针，NULL表示不聚合
 * @return
 */
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
extern "C"
#ifdef __WIN32__
MUXERSHARED_EXPORT
#else
#endif // _WIN32
IUNknown *CreateInstance(IUNknown* pUnknowOuter = nullptr);
#endif
#endif // MUXERFFMPEG_H
