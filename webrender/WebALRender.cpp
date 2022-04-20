#include "WebALRender.h"
#include <iostream>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>

#define MAX_BUFFERS 7

using namespace emscripten;

WebALRender::WebALRender()
    : m_pDevice(nullptr), m_Context(nullptr), m_isInit(false), m_speed(1.0)

{
    init();
}

WebALRender::~WebALRender()
{
}

void WebALRender::init()
{
    m_pDevice = alcOpenDevice(nullptr);
    if (m_pDevice)
    {
        m_Context = alcCreateContext(m_pDevice, nullptr);
        alcMakeContextCurrent(m_Context);
    }
    else
    {
        std::cout << "device open error" << std::endl;
    }

    alGenSources(1, &m_outSourceId);
    alSpeedOfSound(1.0);
    alSourcef(m_outSourceId, AL_PITCH, 1.0f);
    alSourcef(m_outSourceId, AL_GAIN, 1.0f);                         //设置音量大小，1.0f表示最大音量。openAL动态调节音量大小就用这个方法
    alSourcei(m_outSourceId, AL_LOOPING, AL_FALSE);
    alSourcef(m_outSourceId, AL_SOURCE_TYPE, AL_STREAMING);
}

void WebALRender::cleanUpOpenAL()
{
    alDeleteSources(1, &m_outSourceId);                               //删除声源
    if (m_Context != nullptr) {
        alcDestroyContext(m_Context);                                 //删除环境
        m_Context = nullptr;
    }
    if (m_pDevice != nullptr) {
        
        alcCloseDevice(m_pDevice);                                    //关闭设备
        m_pDevice = nullptr;
    } 
}

void WebALRender::cleanBuffer()
{
    int nProcessed;
    alGetSourcei(m_outSourceId, AL_BUFFERS_PROCESSED, &nProcessed);    //获取处理队列，得出已经播放过的缓冲器的数量 
    while (nProcessed--)                                               //将已经播放过的的数据删除掉
    {
        ALuint buff;
        alSourceUnqueueBuffers(m_outSourceId, 1, &buff);               //更新缓存buffer中的数据到source中
        alDeleteBuffers(1, &buff);                                     //删除缓存buff中的数据
    }
}

void WebALRender::playSound()
{
    int ret = 0;
    ALint state;
    alGetSourcei(m_outSourceId, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING)
    {
        alSourcePlay(m_outSourceId);
        if ((ret = alGetError()) != AL_NO_ERROR)
        {
            printf("error alSourcePlay %x\n", ret);
        }
    }
}

void WebALRender::stopSound()
{
    playSound();

    ALint  state;
    alGetSourcei(m_outSourceId, AL_SOURCE_STATE, &state);
    if (state != AL_STOPPED) {
        alSourceStop(m_outSourceId);
    }
}

void WebALRender::setAudioData(std::string data)
{
    m_mutex.lock();
    unsigned char *pData = (unsigned char *)data.c_str();
    ALsizei dataSize = data.size();

    int ret = 0;
    ALuint bufferID = 0;                                                  //buffer id 负责缓存,要用局部变量每次数据都是新的地址

    alGenBuffers(1, &bufferID);                                           //创建一个buffer
    if ((ret = alGetError()) != AL_NO_ERROR)
    {
        m_mutex.unlock();
        printf("error alGenBuffers %x \n", ret);
        return;
    }

    //指定要将数据复制到缓冲区中的数据
    alBufferData(bufferID, m_nAudioFormat, (char *)pData, dataSize, m_nSampleRate * m_speed);
    if ((ret = alGetError()) != AL_NO_ERROR)
    {
        m_mutex.unlock();
        printf("error alBufferData %x\n", ret);
        return;
    }

    cleanBuffer();

    int nQueued;
    alGetSourcei(m_outSourceId, AL_BUFFERS_QUEUED, &nQueued);             //获取缓存队列，缓存的队列数量
    if (nQueued < MAX_BUFFERS)                                            //如果缓冲队列大于MAX_BUFFERS则将该包数据抛弃，不添加进入缓冲区，不进行播放。                                                              
    {                                                                     //目的是降低延迟，例如：延迟时间控制在210ms，每30ms发送一包，则MAX_BUFFERS=7
        alSourceQueueBuffers(m_outSourceId, 1, &bufferID);                //将缓存添加到声源上（添加便会进行播放，不添加不播放）
        if ((ret = alGetError()) != AL_NO_ERROR)
        {
            m_mutex.unlock();
            printf("error alSourceQueueBuffers %x\n", ret);
            return;
        }
    }

    if ((ret = alGetError()) != AL_NO_ERROR)                              //判断是否有错，有错误则清除缓存
    {
        m_mutex.unlock();
        printf("error play failed %x\n", ret);
        alDeleteBuffers(1, &bufferID);
        return;
    }

    bufferID = 0;
    m_mutex.unlock();

    playSound();

    
    
}

void WebALRender::setAudioFormat(int nSampleRate, int audioFormat)
{
    m_nSampleRate = nSampleRate;
    m_nAudioFormat = AL_FORMAT_STEREO16;
}

float WebALRender::volume()
{
    return m_volume;
}
void WebALRender::setVolume(float volume)
{
    m_volume = volume;
    alSourcef(m_outSourceId, AL_GAIN, m_volume);
}

void WebALRender::setSpeed(float speed)
{
    m_speed = speed;
}

void WebALRender::clearRenderer()
{
    m_isInit = false;
    cleanUpOpenAL();
}

bool WebALRender::isInit()
{
    return m_isInit;
}

EMSCRIPTEN_BINDINGS(WebALRender)
{
    class_<WebALRender>("WebALRender")
        .constructor<>()
        .function("isInit", &WebALRender::isInit)
        .function("setAudioData", &WebALRender::setAudioData)
        .function("setAudioFormat", &WebALRender::setAudioFormat)
        .function("volume", &WebALRender::volume)
        .function("setVolume", &WebALRender::setVolume)
        .function("setSpeed", &WebALRender::setSpeed)
        .function("clearRenderer", &WebALRender::clearRenderer);
}
