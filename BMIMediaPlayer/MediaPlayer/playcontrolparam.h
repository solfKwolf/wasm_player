#ifndef PLAYCONTROLPARAM
#define PLAYCONTROLPARAM
#include <mutex>
#include <condition_variable>

class SeekInfo
{
public:
    SeekInfo() : m_needSeek(false), m_nSeekSecond(0){}

    void init()
    {
        m_needSeek = false;
        m_nSeekSecond = 0;
    }

    bool m_needSeek;
    int m_nSeekSecond;
};

class ScaleInfo
{
public:
    ScaleInfo() : m_needScale(false), m_fScale(1.0){}

    void init()
    {
        m_needScale = false;
        m_fScale = 1.0;
    }

    bool m_needScale;
    float m_fScale;
};

class PauseInfo
{
public:
    PauseInfo() : m_pauseForcewake(false), m_isPause(false){}

    void waitPause()
    {
        std::unique_lock<std::mutex> locker(m_pauseMutex);
        m_pauseCond.wait(locker, [this]{return !m_isPause || m_pauseForcewake;});
        m_pauseForcewake = false;
    }

    void checkPause()
    {
        m_isPause = false;
        m_pauseCond.notify_all();
    }

    void forcewakePause()
    {
        m_pauseForcewake = true;
        m_pauseCond.notify_all();
    }

    void init()
    {
        m_pauseForcewake = false;
        m_isPause = false;
    }

    std::mutex m_pauseMutex;
    std::condition_variable m_pauseCond;
    bool m_pauseForcewake;
    bool m_isPause;
};

#endif // PLAYCONTROLPARAM

