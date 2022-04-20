#include "audiorender.h"
#include <QDebug>

AudioRender::AudioRender()
    : m_pAudioOutput(nullptr)
    , m_isInit(false)
{

}

AudioRender::~AudioRender()
{
    if(m_pAudioOutput)
    {
        delete m_pAudioOutput;
        m_pAudioOutput = nullptr;
    }
}

void AudioRender::setData(QByteArray buffer)
{
    if(!m_pDevice) return;
    m_pDevice->write(buffer.data(), buffer.length());
}

void AudioRender::setAudioFormat(int nSampleRate, int nChannelCount, int nSampleSize)
{
    m_isInit = true;

    if(m_pAudioOutput)
    {
        delete m_pAudioOutput;
        m_pAudioOutput = nullptr;
    }
    QAudioFormat format;
    format.setSampleRate(nSampleRate);
    format.setChannelCount(nChannelCount);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::UnSignedInt);
    format.setSampleSize(nSampleSize);
    format.setByteOrder(QAudioFormat::LittleEndian);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        return;
    }
    m_pAudioOutput = new QAudioOutput(format);
    if(m_pAudioOutput->state() != QAudio::ActiveState)
    {
        m_pDevice = m_pAudioOutput->start();
    }

}

bool AudioRender::isInit()
{
    return m_isInit;
}

