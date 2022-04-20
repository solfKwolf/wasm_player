
#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include "basethread.h"
#include "basemediafactory.h"
#include <functional>

typedef std::function<void (IUNknown*)> SendFrameCallBack;
typedef std::function<IUNknown* (int)> GetFrontFrameCallBack;
typedef std::function<void (void)> StopRenderCallBack;
typedef std::function<int (int)> GetPacketCountCallBack;
typedef std::function<void (int)> PositionChangeCallBack;

class RenderThread : public BaseThread
{
public:
    RenderThread(BaseMediaFactory *pMediaFac,int streamIndex);
    ~RenderThread();

    void getFrontFrameCallBack(GetFrontFrameCallBack getFrontFrame);
    void getPacketCountCallBack(GetPacketCountCallBack getPacketCount);
    void positionChangeCallBack(PositionChangeCallBack positionChange);
    void stopRenderCallBack(StopRenderCallBack stopRender);
    void sendFrameCallBack(SendFrameCallBack sendFrame);
    void getPacketBufferSize(int nPacketBuffSize);

    void setStreamCount(int nStreamCount);
    void autoSpeed(bool autoSpeed = true);
    void setSpeed(float fSpeed);
    float getSpeed();
    int64_t getCodeRate();

    void run();

private:
    BaseMediaFactory *m_pMediaFac;
    GetPacketCountCallBack m_getPacketCount;
    GetFrontFrameCallBack m_getFrontFrame;
    PositionChangeCallBack m_positionChange;
    StopRenderCallBack m_stopRender;
    SendFrameCallBack m_sendFrame;
    int m_nStreamIndex;
    int m_packetBufferSize;


};

#endif // RENDERTHREAD_H
