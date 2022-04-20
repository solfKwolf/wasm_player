#include "mediaplayer.h"
#include <iostream>
#include "mediadata.h"
#include "mediafunction.h"
#include "mediacallback.h"

HRESULT MediaPlayer::QueryInterface(const IID &iid, void **ppv)
{
    return m_pUnknowOuter->QueryInterface(iid, ppv);
}

ULONG MediaPlayer::AddRef()
{
    return m_pUnknowOuter->AddRef();
}

ULONG MediaPlayer::Release()
{
    return m_pUnknowOuter->Release();
}

HRESULT MediaPlayer::NondelegatingQueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUNknown)
    {
        *ppv = static_cast<INondelegatingUnknown*>(this);
    }
    else if(iid == IID_IPLAY)
    {
        *ppv = static_cast<IPlay*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUNknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG MediaPlayer::NondelegatingAddRef()
{
    return ++m_cRef;
}

ULONG MediaPlayer::NondelegatingRelease()
{
    if(--m_cRef == 0)
    {
        std::cout << "COM MediaPlayer FFmpeg delete." << std::endl;
        delete this;
        return 0;
    }
    return m_cRef;
}

MediaPlayer::MediaPlayer(IUNknown *pUnknowOuter)
    : m_cRef(0)
    , m_pMediaFunc(new MediaFunction)

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

MediaPlayer::~MediaPlayer()
{
    std::cout<<"MediaPlayer Destructed."<<std::endl;
    if(m_pMediaFunc)
    {
        m_pMediaFunc->stop();
        delete m_pMediaFunc;
        m_pMediaFunc = NULL;
    }
}

void MediaPlayer::Play()
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->Play();
    }
}

void MediaPlayer::PlayFile(const char *fileName)
{
    SetFileName(fileName);
    Play();
}

void MediaPlayer::PlayStream(bool isStream)
{
    if(m_pMediaFunc)
    {
        MediaData::getInstance()->setIODevice(isStream);
        Play();
    }
}

void MediaPlayer::Stop()
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->stop();
        m_pMediaFunc->SetFileName("");
    }
}

void MediaPlayer::Pause()
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->stopRender();
        m_pMediaFunc->pauseDemux();
    }
}

void MediaPlayer::Resume()
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->resume();
    }
}

void MediaPlayer::TogglePause()
{
    if(m_pMediaFunc)
    {
        if(m_pMediaFunc->getRenderIsRunning())
        {
            Pause();
        }
        else
        {
            Resume();
        }
    }
}

bool MediaPlayer::Seek(int percent)
{
    if(!isRunning())
    {
        TogglePause();
    }
    if(m_pMediaFunc)
    {
        return m_pMediaFunc->seek(percent);
    }
    return false;
}

int64_t MediaPlayer::Duration()
{
    if(m_pMediaFunc)
    {
        return m_pMediaFunc->Duration();
    }
    return 0;
}

void MediaPlayer::SetFileName(const char *fileName)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->SetFileName(fileName);
        MediaData::getInstance()->setIODevice(false);
    }
}

char *MediaPlayer::GetFileName()
{
    if(m_pMediaFunc)
    {
        return m_pMediaFunc->GetFileName();
    }
    return nullptr;
}

bool MediaPlayer::isRunning()
{
    if(m_pMediaFunc)
    {
        return m_pMediaFunc->getRenderIsRunning();
    }
    return false;
}

float MediaPlayer::volume()
{
    return MediaData::getInstance()->getVolume();
}

void MediaPlayer::setVolume(float volume)
{
    MediaData::getInstance()->setVolume(volume);
}

void MediaPlayer::setScale(float scale)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->setScale(scale);
    }
}

float MediaPlayer::getScale()
{
    return MediaData::getInstance()->getScale();
}

void MediaPlayer::setSpeed(float fSpeed)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->setSpeed(fSpeed);
    }
}

float MediaPlayer::getSpeed()
{
    return MediaData::getInstance()->getSpeed();
}

void MediaPlayer::autoSpeed(bool autoSpeed)
{
    MediaData::getInstance()->setAutoSpeed(autoSpeed);
}

int64_t MediaPlayer::getCoderate()
{
    if(m_pMediaFunc)
    {
        return m_pMediaFunc->getCoderate();
    }
    return 0;
}

bool MediaPlayer::getHardware()
{
    if(m_pMediaFunc)
    {
        return MediaData::getInstance()->getHardware();
    }
    return false;
}

void MediaPlayer::setCodeRateSecond(int nSecond)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->setCodeRateSecond(nSecond);
    }
}

void MediaPlayer::setClockType(IPlay::ClockType type)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->setClockType(type);
    }
}

void MediaPlayer::setCacheTime(int nSecond)
{
    if(m_pMediaFunc)
    {
        MediaData::getInstance()->setCacheTime(nSecond);
    }
}

void MediaPlayer::setMaxAutoSpeed(float nSpeed)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->setMaxAutoSpeed(nSpeed);
    }
}

void MediaPlayer::setHardware(bool needHardware)
{
    if(m_pMediaFunc)
    {
        MediaData::getInstance()->setHardware(needHardware);
    }
}

void MediaPlayer::setAudioRenderFormat(FUN_SetAudioFormat audioFormat)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->setAudioFormatCallBack(audioFormat);
    }
}

void MediaPlayer::setAudioRenderData(FUN_SetAudioData audioData)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->setAudioRenderData(audioData);
    }
}

void MediaPlayer::setVideoRenderFormat(FUN_SetVideoFormat videoFormat)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->setVideoFormatCallBack(videoFormat);
    }
}

void MediaPlayer::createVideoFrameOver(FUN_CreateFrameFinish createFrameFinish)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->createVideoFrameOver(createFrameFinish);
    }
}

void MediaPlayer::getVideoFrameBuffer(FUN_GetVideoBuffer videoBuffer)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->getVideoFrameBuffer(videoBuffer);
    }
}

void MediaPlayer::clearRenderer(FUN_ClearRenderer renderer)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->clearRendererCallBack(renderer);
    }
}

void MediaPlayer::positionChange(FUN_PositionChange positionChange)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->positionChangeCallBack(positionChange);
    }
}

void MediaPlayer::openFinishType(FUN_OpenFinishType openFinish)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->openFinishTypeCallBack(openFinish);
    }
}

void MediaPlayer::playFinish(FUN_PlayFinish playFinish)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->playFinishCallBack(playFinish);
    }
}

void MediaPlayer::playStop(FUN_PlayStop playStop)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->playStopCallBack(playStop);
    }
}

void MediaPlayer::setSeekCallBack(FUN_Seek seekCallback)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->setSeekCallBack(seekCallback);
    }
}

void MediaPlayer::setQRCodeText(const char *strText)
{
    if(m_pMediaFunc)
    {
        std::string text;
        if(strText)
            text = strText;
        m_pMediaFunc->setQRCodeText(text);
    }
}

void MediaPlayer::setIOCallBack(FUN_IO inputCallback)
{
    if(m_pMediaFunc)
    {
        m_pMediaFunc->getCallBack()->setIOCallBack(inputCallback);
    }
}

IUNknown *CreateInstance(IUNknown *pUnknowOuter)
{
    std::cout<<"CreateInstance of MediaPlayer"<<std::endl;
    IUNknown* pI = (IUNknown*)(void*)new MediaPlayer(pUnknowOuter);
    pI->AddRef();
    return pI;
}


