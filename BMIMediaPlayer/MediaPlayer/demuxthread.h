#ifndef DEMUXTHREAD_H
#define DEMUXTHREAD_H

#include "basethread.h"
#include "basemediafactory.h"
#include "playcontrolparam.h"
#include <functional>

typedef std::function<void(IUNknown*)> OnRecvPacketCallBack;
typedef std::function<void(void)> ClearPacketCallBack;

class DemuxThread : public BaseThread
{
public:
    DemuxThread(BaseMediaFactory *mediaFac,
                IUNknown* pDemuxerUnknow,
                OnRecvPacketCallBack recvPacket);
    virtual ~DemuxThread();

    virtual void run();

    void controlDemux();
    void pause();
    void resume();
    bool seek(int nSecond);
    void setScale(float scale);
    void forceWakePause();
    void clearPacket(ClearPacketCallBack clearPacket);

protected:
    IUNknown* m_pDemuxerUnknow;
    BaseMediaFactory *m_pMediaFac;
    OnRecvPacketCallBack m_recvPacket;
    ClearPacketCallBack m_clearPacket;
    std::mutex m_mutex;
    std::mutex m_seekMutex;

    SeekInfo m_seekInfo;
    ScaleInfo m_scaleInfo;
    PauseInfo m_pauseInfo;

};

#endif // DEMUXTHREAD_H
