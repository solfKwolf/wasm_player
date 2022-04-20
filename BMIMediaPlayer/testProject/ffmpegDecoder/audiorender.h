#ifndef AUDIORENDER_H
#define AUDIORENDER_H
#include <QBuffer>
#include <QAudioOutput>
#include <QMutex>

class AudioRender
{
public:
    AudioRender();
    ~AudioRender();

    void setData(QByteArray buffer);
    void setAudioFormat(int nSampleRate = 44100, int nChannelCount = 2, int nSampleSize = 16);
    bool isInit();

private:
    QAudioOutput* m_pAudioOutput;
    QIODevice* m_pDevice;
    QMutex m_mutex;

    bool m_isInit;
};

#endif // AUDIORENDER_H
