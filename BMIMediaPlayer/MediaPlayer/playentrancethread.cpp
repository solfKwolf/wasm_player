#include "playentrancethread.h"

PlayEntranceThread::PlayEntranceThread(SetPlayFun playFun)
    : m_setPlayFun(playFun)
{
}

PlayEntranceThread::~PlayEntranceThread()
{

}

void PlayEntranceThread::kill()
{
    if(this->isRunning())
    {
        wait();
    }
}

void PlayEntranceThread::run()
{
    m_setPlayFun();
}

