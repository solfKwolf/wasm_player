#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include "basethread.h"
#include "basemediafactory.h"
#include "iunknownqueue.h"

typedef std::function<void (IUNknown*)> OnSendFrameCallBack;
typedef std::function<IUNknown* (int)> OnGetPacketCallBack;

class DecodeThread : public BaseThread
{
public:
    DecodeThread(BaseMediaFactory* mediaFac,
                 IUNknown* pDecoderUnknow,
                 OnGetPacketCallBack getPacket,
                 OnSendFrameCallBack sendFrame,
                 int stream_index);
    ~DecodeThread();

    void run();

private:
    IUNknown* m_pDecoderUnknow;
    BaseMediaFactory *m_pMediaFac;
    OnGetPacketCallBack m_getPacket;
    OnSendFrameCallBack m_sendFrame;
    int n_streamIndex;
};

#endif // DECODETHREAD_H
