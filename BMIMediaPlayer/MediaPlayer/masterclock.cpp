#include "masterclock.h"

using namespace std::chrono;
MasterClock* MasterClock::m_pInstance = NULL;
std::mutex MasterClock::m_mutex;

MasterClock::MasterClock()
    : m_fSpeed(1.0)
    , m_lAVPts(0)
    , m_isFirstFrame(true)
    , m_isPause(false)
{

}

MasterClock::~MasterClock()
{

}


MasterClock *MasterClock::getInstance()
{
    m_mutex.lock();
    if(!m_pInstance)
    {
        m_pInstance = new MasterClock;
    }
    m_mutex.unlock();
    return m_pInstance;
}

void MasterClock::seekClock(int nMSecond)
{
    m_lAVPts = nMSecond;
    m_begin = high_resolution_clock::now();
}

void MasterClock::reset()
{
    seekClock(0);
}

long MasterClock::getClock()
{
    return (duration_cast<milliseconds>(high_resolution_clock::now() - m_begin).count())*m_fSpeed + m_lAVPts;
}

void MasterClock::pauseClock()
{
    m_lAVPts = getClock();
    m_isPause = true;
}

void MasterClock::setFirstFrame(bool isFirstFrame)
{
    m_isFirstFrame = isFirstFrame;
}

bool MasterClock::IsFirstFrame()
{
    return m_isFirstFrame;
}

void MasterClock::setSpeed(float fSpeed)
{
    m_fSpeed = fSpeed;
}

float MasterClock::getSpeed()
{
    return m_fSpeed;
}



