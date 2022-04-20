#include "cbasethread.h"
#include <iostream>
#include <functional>

CBaseThread::CBaseThread()
    : m_isRunning(false)
    , m_clearSelf(false)
    , m_nFinishedCount(0)
{

}

CBaseThread::~CBaseThread()
{
    if(m_thread.joinable())
    {
        m_thread.join();
    }
    std::cout << "CBaseThread::~CBaseThread()" << std::endl;
}

void CBaseThread::start()
{
    //m_startmutex.lock();
    if(this->isRunning())
        return;
    m_thread = std::thread(std::bind(&CBaseThread::thread, this));
    m_thread.detach();
}

bool CBaseThread::isRunning()
{
    return m_isRunning;
}

bool CBaseThread::isFinished()
{
    return !m_isRunning;
}

void CBaseThread::needClearSelf()
{
    m_clearSelf = true;
}

bool CBaseThread::wait(unsigned long time)
{
    if(m_threadId == std::this_thread::get_id())
    {
        std::cout << "wait oneslef, thread id is: " << m_threadId << std::endl;
        return false;
    }

#ifdef WIN32
    std::unique_lock<std::mutex> locker(m_startmutex);
    bool bWait = m_mutex.try_lock_for(std::chrono::milliseconds(time));
    if(bWait)
        m_mutex.unlock();
    return bWait;
#else
    bool bWait = false;
    while(!(bWait = m_mutex.try_lock()) && time > 0)
    {
        CBaseThread::msleep(1);
        time--;
    }
    if(bWait)
        m_mutex.unlock();
    return bWait;
#endif
}

void CBaseThread::sleep(unsigned long sec)
{
    std::this_thread::sleep_for(std::chrono::seconds(sec));
}

void CBaseThread::msleep(unsigned long msec)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}

std::thread::id CBaseThread::getThreadID()
{
    return m_threadId;
}

void CBaseThread::finished()
{
}
#include <cassert>
void CBaseThread::thread()
{
    //m_startmutex.unlock();
    {
        std::unique_lock<std::timed_mutex> locker(m_mutex);
        m_threadId = std::this_thread::get_id();
        m_isRunning = true;
        run();
        m_isRunning = false;
        // finish
        finished();
    }
    m_nFinishedCount++;
    std::cout << "finished count is: " << m_nFinishedCount << std::endl;
    if(m_clearSelf)
        delete this;
}
