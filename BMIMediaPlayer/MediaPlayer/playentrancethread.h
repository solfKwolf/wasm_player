#ifndef PLAYENTRANCETHREAD_H
#define PLAYENTRANCETHREAD_H
#include <functional>
#include "basethread.h"

typedef std::function<void (void)> SetPlayFun;

class PlayEntranceThread : public BaseThread
{
public:
    PlayEntranceThread(SetPlayFun playFun);
    ~PlayEntranceThread();

    void run();
    void kill();
private:
    SetPlayFun m_setPlayFun;

};

#endif // PLAYENTRANCETHREAD_H
