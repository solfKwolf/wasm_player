#ifndef FFMEDIAFACTORY_H
#define FFMEDIAFACTORY_H

#include "basemediafactory.h"
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
#include "comdelegate.h"
#else
#endif

struct IUNknown;

class FFMediaFactory : public BaseMediaFactory
{
public:
    FFMediaFactory();
    ~FFMediaFactory();

    virtual IUNknown *createDemuxerUNknown();
    virtual IUNknown *createDecoderUNknown();
    virtual IUNknown *createPacketUNknown();
    virtual IUNknown *createStreamUNknown();
    virtual IUNknown *createFrameUNknown();
    virtual IUNknown *createScaleUNknown();

private:
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
    COMDelegate m_demuxerDelegate;
    COMDelegate m_decoderDelegate;
    COMDelegate m_packetDelegate;
    COMDelegate m_streamDelegate;
    COMDelegate m_frameDelegate;
    COMDelegate m_scaleDelegate;
#else
#endif

};

#endif // ABSTRACTFACTORY_H
