#ifndef MEDIAFUNCTION_H
#define MEDIAFUNCTION_H

#include "demuxthread.h"
#include "decodethread.h"
#include "iunknownqueue.h"
#include "renderthread.h"
#include "play.h"
#include "qrcodetoimg.h"

class IUNknown;
class BaseMediaFactory;
class PlayEntranceThread;
class MediaCallBack;

class MediaFunction
{
public:
    MediaFunction();
    ~MediaFunction();

    void playInThread();
    void openFileSuccess();
    void createDemuxThread(IUNknown* pUNknown);
    void createDecodeThread(IUNknown *pDecoderUnkown, int stream_index);
    void createRenderThread();
    void initIUNknownQueue();
    void recvPacket(IUNknown* pUNknown);
    void recvFrame(IUNknown* pUNknown);
    IUNknown* getPacket(int index);
    IUNknown* getFrontFrame(int index);

    void clearPacket();
    void clearFrame();
    void clearData();
    void Play();
    void stop();
    void resume();
    bool seek(int percent);
    void setScale(float scale);
    void setSpeed(float fSpeed);
    void pauseDemux();
    void forcewakeQueue();
    int getPacketCount(int nIndex);
    int64_t Duration();
    int64_t getCoderate();
    void setCodeRateSecond(int nSecond);
    void setClockType(IPlay::ClockType type);
    void setMaxAutoSpeed(float nSpeed);
    void SetFileName(const char* fileName);
    char* GetFileName() ;
    MediaCallBack * getCallBack();
    int64_t readData(unsigned char* buffer, int len);
    int64_t seekData(int64_t offset, int whence);

    void getAudioFormat(int &sampleRate, int &aduioChannels,int &aduioFormat);
    void setAudioData(const unsigned char* pAudioData, int nAudioSize);
    void getVideoFormat(int &nWidth,int &nHeight,int &nFormat);
    void createVideoFrameFinish();
    void positionChange(int position);
    unsigned char* getVideoBuffer();
    bool getRenderIsRunning();
    void stopRender();
    void startRender();
    void releaseRender();
    void playStop();
    void processAudio(IUNknown* pFrameUNknown);
    void processVideo(IUNknown* pFrameUNknown);

    void setQRCodeText(std::string strText);
private:

    BaseMediaFactory *m_pMediaFac;
    PlayEntranceThread *m_pEntrance;
    IUNknown *m_pDemuxerUnknow;
    DemuxThread *m_pDemuxThread;
    MediaCallBack *m_pCallBack;
    std::vector<DecodeThread*> m_DecodeThreadVec;
    RenderThread *m_audioRenderThread;
    RenderThread *m_videoRenderThread;
    IUNknownQueue m_packetQueue;
    IUNknownQueue m_frameQueue;
    std::vector<IUNknown*> m_streamVec;
    std::vector<IUNknown*> m_decoderVec;
    IUNknown *m_pVideoScaleUNknown;
    IUNknown *m_pAudioScaleUNknown;

    QRCodeToImg m_qrcode;

};

#endif // MEDIAFUNCTION_H
