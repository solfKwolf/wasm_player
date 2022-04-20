#ifndef DEMUXERFFMPEG_H
#define DEMUXERFFMPEG_H

#if defined(DEMUXER_LIBRARY)
#  define DEMUXERSHARED_EXPORT __declspec(dllexport)
#else
#  define DEMUXERSHARED_EXPORT __declspec(dllimport)
#endif

extern "C"
{
    struct AVFormatContext;
    struct AVIOContext;
    struct AVPacket;
    struct AVBSFContext;
    struct AVStream;
    struct AVCodecParameters;
}

#include <unknown.h>
#include <demux.h>
#include <atomic>

typedef std::shared_ptr<AVFormatContext> AVFormatContextPtr;
typedef std::shared_ptr<AVIOContext> AVIOContextPtr;
typedef std::shared_ptr<AVBSFContext> AVBSFContextPtr;

class DemuxerFFmpeg
        : public INondelegatingUnknown
        , public IDemux
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
    DemuxerFFmpeg(IUNknown* pUnknowOuter);
    /**
     * @brief 析构函数
     */
    virtual ~DemuxerFFmpeg();

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
    virtual IDemuxError initDemuxer(std::string url) override;
    virtual IDemuxError initDemuxer(IOCallBack inputCallback, SeekCallBack seekCallback = nullptr) override;
    virtual IDemuxError demuxStream(IUNknown* pUnknow) override;
    virtual IDemuxError converToNAL(IUNknown *pDst, IUNknown *pSrc, bool addAACHeader = true) override;
    virtual bool seek(int nSecond) override;
    virtual void setScale(float scale) override;
    virtual int64_t duration() override;
    virtual void pause() override;
    virtual void resume() override;
    virtual int getStreamCount() override;
    virtual IDemuxError getStream(int nStreamIndex, IUNknown* pIStream) override;

public:
    int64_t readData(unsigned char* buffer, int size);
    int64_t seekData(int64_t offset, int whence);
    void codecParFFmpeg2Media(AVCodecParameters *par, IUNknown* pIStream);
    void clearDemuxer();

private:
    void addAACHead(unsigned char *pData, AVCodecParameters *pContext, int packetSize);
    void getPacket(AVPacket *pkt, IUNknown *pIPacket);
    AVBSFContext* createBSFContext(std::string bsfName, AVStream *inStream);

private:
    enum
    {
        WriteBufferSize = 1024 * 1024 * 4,
    };
    AVFormatContextPtr m_pInputContext;

    // 输入为回调函数时使用
    AVIOContextPtr m_pIOContext;
    IOCallBack m_inputCallback;
    SeekCallBack m_seekCallBack;
    std::shared_ptr<unsigned char> m_pWriteBuffer;

    // 转换Packet时使用
    AVBSFContextPtr m_bsfContext;
    AVBSFContextPtr m_hevcBsfContext;
    AVBSFContextPtr m_aacBsfContext;
};

/**
 * @brief CreateInstance
 * @param pUnknowOuter 外部组件Unknown接口指针，NULL表示不聚合
 * @return
 */
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
extern "C"
#ifdef __WIN32__
DEMUXERSHARED_EXPORT
#else
#endif // _WIN32
IUNknown *CreateInstance(IUNknown* pUnknowOuter = nullptr);
#endif

#endif // DEMUXERFFMPEG_H
