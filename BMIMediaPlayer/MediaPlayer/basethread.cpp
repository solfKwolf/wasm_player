#include "basethread.h"
#include <iostream>
#include <frame.h>
#include "masterclock.h"
#include "renderparamcontrol.h"

BaseThread::BaseThread()
    : m_bStopFlag(true)
{

}

BaseThread::~BaseThread()
{

}

void BaseThread::run()
{

}

void BaseThread::start()
{
    m_bStopFlag = false;
    CBaseThread::start();
}

void BaseThread::stop()
{
    m_bStopFlag = true;
}

bool BaseThread::isStop()
{
    return m_bStopFlag;
}



