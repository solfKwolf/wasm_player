#ifndef MASTERCLOCK_H
#define MASTERCLOCK_H

#include <chrono>
#include <mutex>

class MasterClock
{
    MasterClock();
    class MasterClockPrivate
    {
    public:
        ~MasterClockPrivate()
        {
            if(MasterClock::m_pInstance)
                delete MasterClock::m_pInstance;
        }
    };

public:
    ~MasterClock();
    static MasterClock* getInstance();

    void seekClock(int nMSecond);
    void reset();
    long getClock();
    void pauseClock();
    void setFirstFrame(bool isFirstFrame);
    bool IsFirstFrame();
    void setSpeed(float fSpeed);
    float getSpeed();

private:
    static MasterClockPrivate m_private;
    static MasterClock* m_pInstance;
    static std::mutex m_mutex;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
    float m_fSpeed;
    long m_lAVPts;
    bool m_isFirstFrame;
    bool m_isPause;

};

#endif // MASTERCLOCK_H
