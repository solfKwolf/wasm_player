#ifndef SUBTITLERENDERTHREAD_H
#define SUBTITLERENDERTHREAD_H

#include "basethread.h"
#include "basemediafactory.h"
#include "masterclock.h"

class SubtitleRenderThread : public BaseThread
{
public:
    SubtitleRenderThread();
    ~SubtitleRenderThread();

    void run();

private:
    int m_nStreamIndex;
};

#endif // SUBTITLERENDERTHREAD_H
