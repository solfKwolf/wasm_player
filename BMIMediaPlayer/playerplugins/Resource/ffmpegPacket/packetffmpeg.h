#ifndef PACKETFFMPEG_H
#define PACKETFFMPEG_H

#if defined(PACKET_LIBRARY)
#  define PACKETSHARED_EXPORT __declspec(dllexport)
#else
#  define PACKETSHARED_EXPORT __declspec(dllimport)
#endif

#include <unknown.h>
#include <packet.h>
#include <atomic>

class PacketFFmpeg
        : public INondelegatingUnknown
        , public IPacket
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
    PacketFFmpeg(IUNknown* pUnknowOuter);
    /**
     * @brief 析构函数
     */
    virtual ~PacketFFmpeg();

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
    void setPts(int64_t nMSec) override;
    int64_t getPts() override;
    void setDts(int64_t nMSec) override;
    int64_t getDts() override;
    void setPacketData(unsigned char* packetData, int packetSize) override;
    const unsigned char* getPacketData(int& packetSize) override;
    void setKeyframeFlag(bool isKeyframe) override;
    bool isKeyframe() override;
    void setStreamIndex(int streamIndex) override;
    int getStreamIndex() override;
    void setEofFlag(bool isEnd) override;
    bool isEnd() override;
    std::string packetName() override;
    virtual void setPacketType(AVMediaType mediaType) override;
    virtual AVMediaType getPacketType() override;

    virtual int getPacketPos() override;
    virtual void setPacketPos(int packetPos) override;
    virtual int getPacketDuration() override;
    virtual void setPacketDuration(int packetDuration) override;
    virtual int getPacketFlag() override;
    virtual void setPacketFlag(int packetFlag) override;

private:
    std::shared_ptr<void> m_pPrivateData;
    int64_t m_pts;
    int64_t m_dts;
    unsigned char* m_pPacketData;
    int m_nPacketSize;
    bool m_isKeyframe;
    int m_nStreamIndex;
    bool m_isEnd;
    AVMediaType m_mediaType;

    int m_nPacketPos;
    int m_nPacketDuration;
    int m_nPacketFlag;
};

/**
 * @brief CreateInstance
 * @param pUnknowOuter 外部组件Unknown接口指针，NULL表示不聚合
 * @return
 */
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
extern "C"
#ifdef __WIN32__
PACKETSHARED_EXPORT
#else
#endif // _WIN32
IUNknown *CreateInstance(IUNknown* pUnknowOuter = nullptr);
#endif
#endif // PACKETFFMPEG_H
