#ifndef CBASETHREAD_H
#define CBASETHREAD_H

#include <thread>
#include <mutex>
#include <condition_variable>

class CBaseThread
{
public:
    CBaseThread();
    virtual ~CBaseThread();

    // 开启线程
    void start();

    // 线程是否运行
    bool isRunning();
    bool isFinished();
    void needClearSelf();

    bool wait(unsigned long time = -1);
    static void sleep(unsigned long sec);
    static void msleep(unsigned long msec);
    std::thread::id getThreadID();

protected:
    /**
     * @brief run a pure function to be executed in thread body
     */
    virtual void run() = 0;
    virtual void finished();

private:
    void thread();

private:
    bool m_isRunning;
    bool m_clearSelf;
    int m_nFinishedCount;

    std::thread m_thread;
    std::mutex m_startmutex;
    std::timed_mutex m_mutex;
    std::thread::id m_threadId;
};

#endif // CBASETHREAD_H
