#ifndef AUDIORENDER_H
#define AUDIORENDER_H

#include <QMutex>
#include <QBuffer>
#include <QAudioOutput>
#include "frame.h"

class AudioRender
{
public:
    AudioRender();
    ~AudioRender();

    void setData(QByteArray buffer);

    void setAudioFormat(int &nSampleRate, int &nChannelCount, int &nSampleSize);
    float volume();
    void setVolume(float);
    void clearRenderer();
    bool isInit();

protected:
    void update();

private:
    QAudioOutput* m_pAudioOutput;
    QIODevice* m_pDevice;
    QMutex m_mutex;
    bool m_isInit;
};

#endif // AUDIORENDER_H
