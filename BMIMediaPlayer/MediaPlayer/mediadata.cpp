#include "mediadata.h"
#include <iostream>

MediaData* MediaData::m_pInstance = NULL;
std::mutex MediaData::m_mutex;

MediaData::MediaData()
    : m_FileName(std::string())
    , m_nStreamCount(-1)
    , m_nVStreamIndex(-1)
    , m_nAStreamIndex(-1)
    , m_nSStreamIndex(-1)
    , m_releaseIndex(0)
    , m_nSampleRate(-1)
    , m_nChannelCount(-1)
    , m_nSampleFormat(-1)
    , m_fVolume(1.0)
    , m_fScale(1.0)
    , m_fSpeed(1.0)
    , m_autoSpeed(false)
    , m_needHardWare(false)
    , m_isIODevice(false)
    , m_maxFramelistSize(1)
    , m_maxPacketlistSize(25)
    , m_nFrameRate(0)
    , m_nCacheTime(2)
    , m_isEnableCache(false)
    , m_isRepeat(true)
{

}

MediaData::~MediaData()
{

}

MediaData *MediaData::getInstance()
{
    m_mutex.lock();
    if(!m_pInstance)
    {
        m_pInstance = new MediaData;
    }
    m_mutex.unlock();
    return m_pInstance;
}

void MediaData::setFilename(std::string filename)
{
    m_FileName = filename;
}

std::string MediaData::getFilename()
{
    return m_FileName;
}

void MediaData::setStreamCount(int count)
{
    m_nStreamCount = count;
}

int MediaData::getStreamCount()
{
    return m_nStreamCount;
}

void MediaData::clearStreamIndex()
{
    m_nStreamCount = -1;
    m_nVStreamIndex = -1;
    m_nAStreamIndex = -1;
    m_nSStreamIndex = -1;
    m_isRepeat = true;
}

void MediaData::addReleaseIndex()
{
    m_releaseIndex++;
}

void MediaData::resetReleaseIndex()
{
    m_releaseIndex = 0;
}

int MediaData::getReleaseIndex()
{
    return m_releaseIndex;
}

void MediaData::setVStreamIndex(int index)
{
    m_nVStreamIndex = index;
}

int MediaData::getVStramIndex()
{
    return m_nVStreamIndex;
}

void MediaData::setAStreamIndex(int index)
{
    m_nAStreamIndex = index;
}

int MediaData::getAStramIndex()
{
    return m_nAStreamIndex;
}

void MediaData::setSStreamIndex(int index)
{
    m_nSStreamIndex = index;
}

int MediaData::getSStramIndex()
{
    return m_nSStreamIndex;
}

void MediaData::setAutoSpeed(bool autoSpeed)
{
    m_autoSpeed = autoSpeed;
}

bool MediaData::getAutoSpeed()
{
    return m_autoSpeed;
}

void MediaData::setSpeed(float speed)
{
    m_fSpeed = speed;
}

float MediaData::getSpeed()
{
    return m_fSpeed;
}

void MediaData::setScale(float scale)
{
    m_fScale = scale;
}

float MediaData::getScale()
{
    return m_fScale;
}

void MediaData::setVolume(float volume)
{
    m_fVolume = volume;
}

float MediaData::getVolume()
{
    return m_fVolume;
}

void MediaData::setAudioFormat(int nSampleRate, int nChannelCount, int nSampleSize)
{
    m_nSampleRate = nSampleRate;
    m_nChannelCount = nChannelCount;
    m_nSampleFormat = nSampleSize;
}

void MediaData::getAudioFormat(int &nSampleRate, int &nChannelCount, int &nSampleSize)
{
    nSampleRate = m_nSampleRate;
    nChannelCount = m_nChannelCount;
    nSampleSize = m_nSampleFormat;
}

void MediaData::setVideoFormat(int nWidth, int nHeight, int nFormat)
{
    m_nVideoWidth = nWidth;
    m_nVideoHeight = nHeight;
    m_nPixelFormat = nFormat;
}

void MediaData::getVideoFormat(int &nWidth, int &nHeight, int &nFormat)
{
    nWidth = m_nVideoWidth;
    nHeight = m_nVideoHeight;
    nFormat = m_nPixelFormat;
}

void MediaData::setHardware(bool needHardware)
{
    m_needHardWare = needHardware;
}

bool MediaData::getHardware()
{
    return m_needHardWare;
}

void MediaData::setIODevice(bool isIODevice)
{
    m_isIODevice = isIODevice;
}

bool MediaData::getIODevice()
{
    return m_isIODevice;
}

void MediaData::setMaxFramelistSize(int framelistsize)
{
    m_maxFramelistSize = framelistsize;
}

int MediaData::getMaxFramelistSize()
{
    return m_maxFramelistSize;
}

void MediaData::setMaxPacketlistSize(int packetlistSize)
{
    m_maxPacketlistSize = packetlistSize;
}

int MediaData::getMaxPacketlistSize()
{
    return m_maxPacketlistSize;
}

void MediaData::setFrameRate(int nFrameRate)
{
     m_nFrameRate = nFrameRate;
}

int MediaData::getFrameRate()
{
    return m_nFrameRate;
}

void MediaData::isEnableCache(bool enabled)
{
    m_isEnableCache = enabled;
}

void MediaData::setCacheTime(int nSecond)
{
    m_isEnableCache = true;
    m_nCacheTime = nSecond;
    m_maxPacketlistSize = 25;
    m_maxPacketlistSize = m_nCacheTime*m_maxPacketlistSize;
}

int MediaData::getCacheTime()
{
    return m_nCacheTime;
}

MediaData::PlayStatus MediaData::judgePlayStatus(int nVideoSize, int nAudioSize)
{
    if(!m_isEnableCache)
    {
        return Normal;
    }

    if(nAudioSize == 0 || nVideoSize == 0)
    {
        if(m_isRepeat)
        {
            m_isRepeat = false;
            return Pause;
        }
    }
    else if(nAudioSize == getMaxPacketlistSize() || nVideoSize == getMaxPacketlistSize())
    {
        if(!m_isRepeat)
        {
            m_isRepeat = true;
            return Play;
        }
    }
    return Normal;
}


