#ifndef WEBALRENDER_H
#define WEBALRENDER_H

#include <stdio.h>
#include <string>
#include <mutex>
#include <AL/al.h>
#include <AL/alc.h>

class WebALRender {
public:
    WebALRender();
    ~WebALRender();

    bool isInit();
    void setAudioData(std::string data);
    void setAudioFormat(int nSampleRate, int audioFormat);
    float volume();
    void setVolume(float volume);
    void setSpeed(float speed);
    void clearRenderer();

private:
    void init();
    void cleanUpOpenAL();
    void cleanBuffer();
    void playSound();
    void stopSound();

private:
    ALCdevice  *m_pDevice;
    ALCcontext *m_Context; 
    ALuint      m_outSourceId;

    float m_volume;                 //当前音量volume取值范围(0~1)
    float m_speed;

    int m_nSampleRate;
    int m_nAudioFormat;

    bool m_isInit;
    std::mutex m_mutex;
};

#endif // WEBALRENDER_H