#include "packetffmpeg.h"
#include <iostream>

HRESULT PacketFFmpeg::QueryInterface(const IID &iid, void **ppv)
{
    return m_pUnknowOuter->QueryInterface(iid, ppv);
}

ULONG PacketFFmpeg::AddRef()
{
    return m_pUnknowOuter->AddRef();
}

ULONG PacketFFmpeg::Release()
{
    return m_pUnknowOuter->Release();
}

HRESULT PacketFFmpeg::NondelegatingQueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUNknown)
    {
        *ppv = static_cast<INondelegatingUnknown*>(this);
    }
    else if(iid == IID_IPACKET)
    {
        *ppv = static_cast<IPacket*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUNknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG PacketFFmpeg::NondelegatingAddRef()
{
    return ++m_cRef;
}

ULONG PacketFFmpeg::NondelegatingRelease()
{
    if(--m_cRef == 0)
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

PacketFFmpeg::PacketFFmpeg(IUNknown *pUnknowOuter)
    : m_cRef(0)
    , m_pts(0)
    , m_dts(0)
    , m_pPacketData(nullptr)
    , m_nPacketSize(0)
    , m_isKeyframe(false)
    , m_nStreamIndex(-1)
    , m_isEnd(false)
    , m_mediaType(AVTypeUnknow)
    , m_nPacketPos(0)
    , m_nPacketDuration(0)
    , m_nPacketFlag(0)
{
    if(pUnknowOuter)
    {
        // 被聚合的，使用外部unknown
        m_pUnknowOuter = pUnknowOuter;
    }
    else
    {
        // 没被聚合，使用非代理unknown
        m_pUnknowOuter = reinterpret_cast<IUNknown*>(static_cast<INondelegatingUnknown*>(this));
    }
}

PacketFFmpeg::~PacketFFmpeg()
{
}

void PacketFFmpeg::setPrivateData(void *privateData, ReleasePrivateCallback callback)
{
    m_pPrivateData.reset(privateData, callback);
}

void PacketFFmpeg::resetPrivateData()
{
    m_pPrivateData.reset();
}

void PacketFFmpeg::setPts(int64_t nMSec)
{
    m_pts = nMSec;
}

int64_t PacketFFmpeg::getPts()
{
    return m_pts;
}

void PacketFFmpeg::setDts(int64_t nMSec)
{
    m_dts = nMSec;
}

int64_t PacketFFmpeg::getDts()
{
    return m_dts;
}

void PacketFFmpeg::setPacketData(unsigned char *packetData, int packetSize)
{
    m_pPacketData = packetData;
    m_nPacketSize = packetSize;
}

const unsigned char *PacketFFmpeg::getPacketData(int &packetSize)
{
    packetSize = m_nPacketSize;
    return m_pPacketData;
}

void PacketFFmpeg::setKeyframeFlag(bool isKeyframe)
{
    m_isKeyframe = isKeyframe;
}

bool PacketFFmpeg::isKeyframe()
{
    return m_isKeyframe;
}

void PacketFFmpeg::setStreamIndex(int streamIndex)
{
    m_nStreamIndex = streamIndex;
}

int PacketFFmpeg::getStreamIndex()
{
    return m_nStreamIndex;
}

void PacketFFmpeg::setEofFlag(bool isEnd)
{
    m_isEnd = isEnd;
}

bool PacketFFmpeg::isEnd()
{
    return m_isEnd;
}

std::string PacketFFmpeg::packetName()
{
    return "FFMPEG PACKET";
}

void PacketFFmpeg::setPacketType(IPacket::AVMediaType mediaType)
{
    m_mediaType = mediaType;
}

IPacket::AVMediaType PacketFFmpeg::getPacketType()
{
    return m_mediaType;
}

int PacketFFmpeg::getPacketPos()
{
    return m_nPacketPos;
}

void PacketFFmpeg::setPacketPos(int packetPos)
{
    m_nPacketPos = packetPos;
}

int PacketFFmpeg::getPacketDuration()
{
    return m_nPacketDuration;
}

void PacketFFmpeg::setPacketDuration(int packetDuration)
{
    m_nPacketDuration = packetDuration;
}

int PacketFFmpeg::getPacketFlag()
{
    return m_nPacketFlag;
}

void PacketFFmpeg::setPacketFlag(int packetFlag)
{
    m_nPacketFlag = packetFlag;
}

#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
IUNknown *CreateInstance(IUNknown *pUnknowOuter)
{
    IUNknown* pI = (IUNknown*)(void*)new PacketFFmpeg(pUnknowOuter);
    pI->AddRef();
    return pI;
}
#endif
