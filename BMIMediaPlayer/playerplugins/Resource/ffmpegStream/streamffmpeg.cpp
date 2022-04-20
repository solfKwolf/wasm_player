#include "streamffmpeg.h"
#include <iostream>

HRESULT StreamFFmpeg::QueryInterface(const IID &iid, void **ppv)
{
    return m_pUnknowOuter->QueryInterface(iid, ppv);
}

ULONG StreamFFmpeg::AddRef()
{
    return m_pUnknowOuter->AddRef();
}

ULONG StreamFFmpeg::Release()
{
    return m_pUnknowOuter->Release();
}

HRESULT StreamFFmpeg::NondelegatingQueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUNknown)
    {
        *ppv = static_cast<INondelegatingUnknown*>(this);
    }
    else if(iid == IID_ISTREAM)
    {
        *ppv = static_cast<IAVStream*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUNknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG StreamFFmpeg::NondelegatingAddRef()
{
    return ++m_cRef;
}

ULONG StreamFFmpeg::NondelegatingRelease()
{
    if(--m_cRef == 0)
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

StreamFFmpeg::StreamFFmpeg(IUNknown *pUnknowOuter)
    : m_cRef(0)
    , m_mediaType(AVTypeUnknow)
    , m_duration(0)
    , m_streamIndex(-1)
    , m_pPrivateData(nullptr)
    , m_bitrate(0)
    , m_videoFrameFormat(FormatNone)
    , m_nSampleFormat(AudioFormateNone)
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

StreamFFmpeg::~StreamFFmpeg()
{
}

void StreamFFmpeg::setStreamType(IAVStream::AVMediaType mediaType)
{
    m_mediaType = mediaType;
}

IAVStream::AVMediaType StreamFFmpeg::getStreamType()
{
    return m_mediaType;
}

void StreamFFmpeg::setStreamCodecType(IAVStream::AVCodecType type)
{
    m_codecType = type;
}

IAVStream::AVCodecType StreamFFmpeg::getStreamCodecType()
{
    return m_codecType;
}

int64_t StreamFFmpeg::duration()
{
    return m_duration;
}

void StreamFFmpeg::setDuration(int64_t duration)
{
    m_duration = duration;
}

void StreamFFmpeg::setStreamIndex(int streamIndex)
{
    m_streamIndex = streamIndex;
}

int StreamFFmpeg::getStreamIndex()
{
    return m_streamIndex;
}

void StreamFFmpeg::setPrivateData(void *privateData, IAVStream::ReleasePrivateCallback callback)
{
    m_pPrivateData.reset(privateData, callback);
}

std::string StreamFFmpeg::streamName()
{
    return "FFMPEG STREAM";
}

std::string StreamFFmpeg::getStreamTitle()
{
    return m_streamTitle;
}

void StreamFFmpeg::setStreamTitle(std::string streamTitle)
{
    m_streamTitle = streamTitle;
}

void StreamFFmpeg::getFrameSize(int &nWidth, int &nHeight)
{
    nWidth = m_nWidth;
    nHeight = m_nHeight;
}

void StreamFFmpeg::setFrameSize(int nWidth, int nHeight)
{
    m_nWidth = nWidth;
    m_nHeight = nHeight;
}

IAVStream::FrameFormat StreamFFmpeg::getFrameFormat()
{
    return m_videoFrameFormat;
}

void StreamFFmpeg::setFrameFormat(IAVStream::FrameFormat format)
{
    m_videoFrameFormat = format;
}

void StreamFFmpeg::getAudioFormat(int &nSampleRate, int &nChannelCount, AudioSampleFormat &nSampleFormat)
{
    nSampleRate = m_nSampleRate;
    nChannelCount = m_nChannelCount;
    nSampleFormat = m_nSampleFormat;
}

void StreamFFmpeg::setAudioFormat(int nSampleRate, int nChannelCount, AudioSampleFormat nSampleFormat)
{
    m_nSampleRate = nSampleRate;
    m_nChannelCount = nChannelCount;
    m_nSampleFormat = nSampleFormat;
}

IAVStream::StreamRational StreamFFmpeg::getTimebase()
{
    return m_timebase;
}

void StreamFFmpeg::setTimebase(IAVStream::StreamRational timebase)
{
    m_timebase = timebase;
}

IAVStream::StreamRational StreamFFmpeg::getFrameRate()
{
    return m_framerate;
}

void StreamFFmpeg::setFrameRate(IAVStream::StreamRational frameRate)
{
    m_framerate = frameRate;
}

int64_t StreamFFmpeg::getBitrate()
{
    return m_bitrate;
}

void StreamFFmpeg::setBitrate(int64_t bitrate)
{
    m_bitrate = bitrate;
}

std::string StreamFFmpeg::getCodecExtradata()
{
    return m_strCodecExtradata;
}

void StreamFFmpeg::setCodecExtradata(std::string codecExtradata)
{
    m_strCodecExtradata = codecExtradata;
}

#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
IUNknown *CreateInstance(IUNknown *pUnknowOuter)
{
    std::cout <<"CreateInstance of StreamFFmpeg" << std::endl;
    IUNknown* pI = (IUNknown*)(void*)new StreamFFmpeg(pUnknowOuter);
    pI->AddRef();
    return pI;
}
#endif

