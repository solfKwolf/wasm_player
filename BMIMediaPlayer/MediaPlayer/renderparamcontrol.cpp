#include "renderparamcontrol.h"
#include "masterclock.h"


RenderParamControl* RenderParamControl::m_pInstance = NULL;
std::mutex RenderParamControl::m_mutex;

RenderParamControl::RenderParamControl()
    : m_seekIndex(0)
    , m_nStreamCount(0)
    , m_speedModel(SpeedNormal)
    , m_autoSpeedFlag(false)
    , m_oldSpeed(0.0)
    , m_multiSpeed(1.1)
    , m_nVideoCodeRate(0)
    , m_nSecond(2)
    , m_clockType(IPlay::VideoClock)
{

}

RenderParamControl::~RenderParamControl()
{

}

RenderParamControl *RenderParamControl::getInstance()
{
    m_mutex.lock();
    if(!m_pInstance)
    {
        m_pInstance = new RenderParamControl;
    }
    m_mutex.unlock();
    return m_pInstance;
}

void RenderParamControl::setStreamCount(int streamCount)
{
    m_nStreamCount = streamCount;
}

int RenderParamControl::getStreamCount()
{
    return m_nStreamCount;
}

void RenderParamControl::addSeekIndex()
{
    m_seekIndex++;
}

void RenderParamControl::resetSeekIndex()
{
    m_seekIndex = 0;
}

bool RenderParamControl::isSeekRender()
{
    if(m_seekIndex == m_nStreamCount)
    {
        return true;
    }
    return false;
}

void RenderParamControl::autoSpeed(bool autoSpeed)
{
    m_autoSpeedFlag = autoSpeed;
}

void RenderParamControl::autoSpeed(int nQueueMax, int nQueueSize)
{
    if(!m_autoSpeedFlag)
    {
        return;
    }
    // 缓存不多时0.9倍速播放
    if(nQueueSize < nQueueMax * 1/3)
    {
        if(m_speedModel == SpeedNormal)
        {
            m_speedModel = SpeedDown;
            m_oldSpeed = MasterClock::getInstance()->getSpeed();
            MasterClock::getInstance()->setSpeed(0.9 * m_oldSpeed);
        }
        else if(m_speedModel == SpeedUp)
        {
            m_speedModel = SpeedDown;
            MasterClock::getInstance()->setSpeed(0.9 * m_oldSpeed);
        }
    }
    // 缓存大于最大缓冲区1/2时1.1倍速播放
    else if(nQueueSize > nQueueMax * 1/2)
    {
        if(m_speedModel == SpeedNormal)
        {
            m_speedModel = SpeedUp;
            m_oldSpeed = MasterClock::getInstance()->getSpeed();
            MasterClock::getInstance()->setSpeed(m_multiSpeed * m_oldSpeed);
        }
        else if(m_speedModel == SpeedDown)
        {
            m_speedModel = SpeedUp;
            MasterClock::getInstance()->setSpeed(m_multiSpeed * m_oldSpeed);
        }
    }
    else if(m_speedModel != SpeedNormal)
    {
        m_speedModel = SpeedNormal;
        MasterClock::getInstance()->setSpeed(m_oldSpeed);
    }
}

bool RenderParamControl::isAutoSpeed()
{
    return m_autoSpeedFlag;
}

int64_t RenderParamControl::getCodeRate()
{
    return m_nVideoCodeRate;
}

void RenderParamControl::setCodeRate(int nCodeRate)
{
    m_nVideoCodeRate = nCodeRate;
}

void RenderParamControl::setCodeRateSecond(int nSecond)
{
    m_nSecond = nSecond;
}

void RenderParamControl::setMaxAutoSpeed(float nSpeed)
{
    m_multiSpeed = nSpeed;
}

int RenderParamControl::getCodeRateSecond()
{
    return m_nSecond;
}

void RenderParamControl::setClockType(IPlay::ClockType type)
{
    m_clockType = type;
}

IPlay::ClockType RenderParamControl::getClockType()
{
    return m_clockType;
}



