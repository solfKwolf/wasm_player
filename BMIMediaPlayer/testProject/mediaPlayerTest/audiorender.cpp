#include "audiorender.h"
#include <QDebug>
AudioRender::AudioRender()
    : m_pAudioOutput(NULL)
    , m_pDevice(NULL)
    , m_isInit(false)
{

}

AudioRender::~AudioRender()
{
    if(m_pAudioOutput)
    {
        delete m_pAudioOutput;
        m_pAudioOutput = NULL;
    }
}

void AudioRender::setData(QByteArray buffer)
{
    if(!m_pDevice) return;
//    m_pDevice->reset();
    m_pDevice->write(buffer.data(), buffer.length());
    update();
}

void AudioRender::setAudioFormat(int &nSampleRate, int &nChannelCount, int &nSampleSize)
{
    m_isInit = true;
    if(m_pAudioOutput)
    {
		delete m_pAudioOutput;
        m_pAudioOutput = NULL;
    }
    QAudioFormat format;
    format.setSampleRate(nSampleRate);
    format.setChannelCount(nChannelCount);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::UnSignedInt);
    nSampleSize = IFrame::AudioFormateS16;
    format.setSampleSize(16);
    format.setByteOrder(QAudioFormat::LittleEndian);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qDebug() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }
    m_pAudioOutput = new QAudioOutput(format);
    if(m_pAudioOutput->state() != QAudio::ActiveState)
    {
        m_pDevice = m_pAudioOutput->start();
    }
}

void AudioRender::update()
{

}

bool AudioRender::isInit()
{
    return m_isInit;
}

float AudioRender::volume()
{
	if (m_pAudioOutput)
	{
		return m_pAudioOutput->volume();
	}
	return 0.0;
}

void AudioRender::setVolume(float volume)
{
	if (m_pAudioOutput)
	{
		return m_pAudioOutput->setVolume(volume);
    }
}

void AudioRender::clearRenderer()
{
    m_isInit = false;
}

