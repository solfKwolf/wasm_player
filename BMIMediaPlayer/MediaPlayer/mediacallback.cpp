#include "mediacallback.h"

MediaCallBack::MediaCallBack()
    : m_setAudioData(NULL)
    , m_setAudioFormat(NULL)
    , m_setVideoFormat(NULL)
    , m_createFrameFinish(NULL)
    , m_videoBuffer(NULL)
    , m_clearRenderer(NULL)
    , m_positionChange(NULL)
    , m_openFinishType(NULL)
    , m_playFinish(NULL)
    , m_playStop(NULL)
    , m_inputCallback(NULL)
    , m_seekCallback(NULL)
{

}

MediaCallBack::~MediaCallBack()
{

}

void MediaCallBack::setAudioFormatCallBack(FUN_SetAudioFormat audioFormat)
{
    m_setAudioFormat = audioFormat;
}

void MediaCallBack::setVideoFormatCallBack(FUN_SetVideoFormat videoFormat)
{
    m_setVideoFormat = videoFormat;
}

void MediaCallBack::clearRendererCallBack(FUN_ClearRenderer renderer)
{
    m_clearRenderer = renderer;
}

void MediaCallBack::positionChangeCallBack(FUN_PositionChange positionChange)
{
    m_positionChange = positionChange;
}

void MediaCallBack::openFinishTypeCallBack(FUN_OpenFinishType openFinishType)
{
    m_openFinishType = openFinishType;
}

void MediaCallBack::playFinishCallBack(FUN_PlayFinish playFinish)
{
    m_playFinish = playFinish;
}

void MediaCallBack::playStopCallBack(FUN_PlayStop playStop)
{
    m_playStop = playStop;
}

void MediaCallBack::setIOCallBack(FUN_IO inputCallback)
{
    m_inputCallback = inputCallback;
}

void MediaCallBack::setSeekCallBack(FUN_Seek seekCallback)
{
    m_seekCallback = seekCallback;
}

void MediaCallBack::setAudioRenderData(FUN_SetAudioData audioData)
{
    m_setAudioData = audioData;
}

void MediaCallBack::createVideoFrameOver(FUN_CreateFrameFinish createFrameFinish)
{
    m_createFrameFinish = createFrameFinish;
}

void MediaCallBack::getVideoFrameBuffer(FUN_GetVideoBuffer videoBuffer)
{
    m_videoBuffer = videoBuffer;
}

void MediaCallBack::setAudioFormat(int &sampleRate, int &aduioChannels, int &aduioFormat)
{
    if(m_setAudioData)
    {
        m_setAudioFormat(sampleRate,aduioChannels,aduioFormat);
    }
}

void MediaCallBack::setAudioData(const unsigned char *pAudioData, int nAudioSize)
{
    if(m_setAudioData)
    {
        m_setAudioData(pAudioData,nAudioSize);
    }
}

void MediaCallBack::setVideoFormat(int &nWidth, int &nHeight, int &nFormat)
{
    if(m_setVideoFormat)
    {
        m_setVideoFormat(nWidth,nHeight,nFormat);
    }
}

void MediaCallBack::createVideoFrameFinish()
{
    if(m_createFrameFinish)
    {
        m_createFrameFinish();
    }
}

unsigned char *MediaCallBack::getVideoBuffer()
{
    if(m_videoBuffer)
    {
        return m_videoBuffer();
    }
    return NULL;
}


void MediaCallBack::clearRenderer()
{
    if(m_clearRenderer)
    {
        m_clearRenderer();
    }
}

void MediaCallBack::positionChange(int position)
{
    if(m_positionChange)
    {
        m_positionChange(position);
    }
}

void MediaCallBack::openFinshType(int type)
{
    if(m_openFinishType)
    {
        m_openFinishType(type);
    }
}

void MediaCallBack::playFinsh()
{
    if(m_playFinish)
    {
        m_playFinish();
    }
}

void MediaCallBack::playStop()
{
    if(m_playStop)
    {
        m_playStop();
    }
}

int64_t MediaCallBack::readData(unsigned char *buffer, int len)
{
    if(m_inputCallback)
    {
        return m_inputCallback(buffer,len);
    }
    return 0;
}

int64_t MediaCallBack::seekData(int64_t offset, int whence)
{
    if(m_seekCallback)
    {
        return m_seekCallback(offset,whence);
    }
    return 0;
}

