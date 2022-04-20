#ifndef BASETHREAD_H
#define BASETHREAD_H

#include "cbasethread.h"

class BaseThread : public CBaseThread
{
public:
    BaseThread();
    virtual ~BaseThread();

    virtual void run();
    virtual void start();
    virtual void stop();

    bool isStop();

private:
    bool m_bStopFlag;

};

#endif // BASETHREAD_H
