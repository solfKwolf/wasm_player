#include "ffmediafactory.h"
#include "demux.h"
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
#else
#include "demuxerffmpeg.h"
#include "decoderffmpeg.h"
#include "packetffmpeg.h"
#include "frameffmpeg.h"
#include "streamffmpeg.h"
#include "swscalerffmpeg.h"
#endif

FFMediaFactory::FFMediaFactory()
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
    :
#ifdef __WIN32__
      m_demuxerDelegate("ffmpegDemuxer.dll")
    , m_decoderDelegate("ffmpegDecoder.dll")
    , m_packetDelegate("ffmpegPacket.dll")
    , m_streamDelegate("ffmpegStream.dll")
    , m_frameDelegate("ffmpegFrame.dll")
    , m_scaleDelegate("ffmpegScale.dll")
#else
     m_demuxerDelegate("libffmpegDemuxer.so")
    , m_decoderDelegate("libffmpegDecoder.so")
    , m_packetDelegate("libffmpegPacket.so")
    , m_streamDelegate("libffmpegStream.so")
    , m_frameDelegate("libffmpegFrame.so")
    , m_scaleDelegate("libffmpegScale.so")
#endif
#endif
{


}

FFMediaFactory::~FFMediaFactory()
{

}

IUNknown *FFMediaFactory::createDemuxerUNknown()
{
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
    return m_demuxerDelegate.createUnknow();
#else
    IUNknown* pI = (IUNknown*)(void*)new DemuxerFFmpeg(nullptr);
    pI->AddRef();
    return pI;
#endif
}

IUNknown *FFMediaFactory::createDecoderUNknown()
{
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
    return m_decoderDelegate.createUnknow();
#else
    IUNknown* pI = (IUNknown*)(void*)new DecoderFFmpeg(nullptr);
    pI->AddRef();
    return pI;
#endif
}

IUNknown *FFMediaFactory::createPacketUNknown()
{
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
    return m_packetDelegate.createUnknow();
#else
    IUNknown* pI = (IUNknown*)(void*)new PacketFFmpeg(nullptr);
    pI->AddRef();
    return pI;
#endif
}

IUNknown *FFMediaFactory::createStreamUNknown()
{
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
    return m_streamDelegate.createUnknow();
#else
    IUNknown* pI = (IUNknown*)(void*)new StreamFFmpeg(nullptr);
    pI->AddRef();
    return pI;
#endif
}

IUNknown *FFMediaFactory::createFrameUNknown()
{
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
    return m_frameDelegate.createUnknow();
#else
    IUNknown* pI = (IUNknown*)(void*)new FrameFFmpeg(nullptr);
    pI->AddRef();
    return pI;
#endif
}

IUNknown *FFMediaFactory::createScaleUNknown()
{
#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
    return m_scaleDelegate.createUnknow();
#else
    IUNknown* pI = (IUNknown*)(void*)new SwscalerFFmpeg(nullptr);
    pI->AddRef();
    return pI;
#endif
}

