#include "frameffmpeg.h"
#include <iostream>

HRESULT FrameFFmpeg::QueryInterface(const IID &iid, void **ppv)
{
    return m_pUnknowOuter->QueryInterface(iid, ppv);
}

ULONG FrameFFmpeg::AddRef()
{
    return m_pUnknowOuter->AddRef();
}

ULONG FrameFFmpeg::Release()
{
    return m_pUnknowOuter->Release();
}

HRESULT FrameFFmpeg::NondelegatingQueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUNknown)
    {
        *ppv = static_cast<INondelegatingUnknown*>(this);
    }
    else if(iid == IID_IFRAME)
    {
        *ppv = static_cast<IFrame*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUNknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG FrameFFmpeg::NondelegatingAddRef()
{
    return ++m_cRef;
}

ULONG FrameFFmpeg::NondelegatingRelease()
{
    if(--m_cRef == 0)
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

FrameFFmpeg::FrameFFmpeg(IUNknown *pUnknowOuter)
    : m_cRef(0)
    , m_pts(0)
    , m_duration(0)
    , m_isKeyframe(false)
    , m_nStreamIndex(-1)
    , m_isEnd(false)
    , m_nWidth(0)
    , m_nHeight(0)
    , m_format(FormatNone)
    , m_mediaType(AVTypeUnknow)
    , m_nSampleRate(0)
    , m_nAudioChannels(0)
    , m_audioSampleFormat(AudioFormateNone)
    , m_nCurrentDataIndex(0)
    , m_nNBSamples(0)
    , m_nFrameRate(0)
    , m_nPacketSize(0)
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

FrameFFmpeg::~FrameFFmpeg()
{
}

void FrameFFmpeg::setPrivateData(void *privateData, ReleasePrivateCallback callback)
{
    m_pPrivateData.reset(privateData, callback);
}

void FrameFFmpeg::resetPrivateData()
{
    m_pPrivateData.reset();
}

void FrameFFmpeg::setPts(int64_t nMSec)
{
    m_pts = nMSec;
}

int64_t FrameFFmpeg::getPts()
{
    return m_pts;
}

void FrameFFmpeg::setStreamIndex(int streamIndex)
{
    m_nStreamIndex = streamIndex;
}

int FrameFFmpeg::getStreamIndex()
{
    return m_nStreamIndex;
}

void FrameFFmpeg::setEofFlag(bool isEnd)
{
    m_isEnd = isEnd;
}

bool FrameFFmpeg::isEnd()
{
    return m_isEnd;
}

std::string FrameFFmpeg::frameName()
{
    return "FFMPEG FRAME";
}

void FrameFFmpeg::setFrameSize(int width, int nHeight)
{
    m_nWidth = width;
    m_nHeight = nHeight;
}

int FrameFFmpeg::frameWidth()
{
    return m_nWidth;
}

int FrameFFmpeg::frameHeight()
{
    return m_nHeight;
}

void FrameFFmpeg::setFrameFormat(IFrame::FrameFormat format)
{
    m_format = format;
}

IFrame::FrameFormat FrameFFmpeg::getFrameFormat()
{
    return m_format;
}

void FrameFFmpeg::setSampleRate(int sampleRate)
{
    m_nSampleRate = sampleRate;
}

int FrameFFmpeg::getSampleRate()
{
    return m_nSampleRate;
}

void FrameFFmpeg::setAudioChannels(int audioChannels)
{
    m_nAudioChannels = audioChannels;
}

int FrameFFmpeg::getAduioChannels()
{
    return m_nAudioChannels;
}

void FrameFFmpeg::setAudioSampleFormat(IFrame::AudioSampleFormat format)
{
    m_audioSampleFormat = format;
}

IFrame::AudioSampleFormat FrameFFmpeg::getAudioSampleFormat()
{
    return m_audioSampleFormat;
}

int FrameFFmpeg::getNBSamples()
{
    return m_nNBSamples;
}

void FrameFFmpeg::setNBSamples(int nbSamples)
{
    m_nNBSamples = nbSamples;
}

int64_t FrameFFmpeg::getFrameRate()
{
    return m_nFrameRate;
}

void FrameFFmpeg::setFrameRate(int nFrameRate)
{
    m_nFrameRate = nFrameRate;
}

void FrameFFmpeg::setPacketSize(int nPacketSize)
{
    m_nPacketSize = nPacketSize;
}

void FrameFFmpeg::setFrameData(unsigned char *frameData, int frameSize)
{
    m_frameData.data = frameData;
    m_frameData.size = frameSize;
}

const unsigned char *FrameFFmpeg::getFrameData(int &frameSize)
{
    frameSize = m_frameData.size;
    return m_frameData.data;
}

int FrameFFmpeg::getPacketSize()
{
    return m_nPacketSize;
}

void FrameFFmpeg::setFrameType(IFrame::AVMediaType mediaType)
{
    m_mediaType = mediaType;
}

IFrame::AVMediaType FrameFFmpeg::getFrameType()
{
    return m_mediaType;
}

void FrameFFmpeg::setDuration(int64_t duration)
{
    m_duration = duration;
}

int64_t FrameFFmpeg::getDuration()
{
    return m_duration;
}

#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
IUNknown *CreateInstance(IUNknown *pUnknowOuter)
{
    IUNknown* pI = (IUNknown*)(void*)new FrameFFmpeg(pUnknowOuter);
    pI->AddRef();
    return pI;
}
#endif

