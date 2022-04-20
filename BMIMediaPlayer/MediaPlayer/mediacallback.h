#ifndef MEDIACALLBACK_H
#define MEDIACALLBACK_H

#include "play.h"

class MediaCallBack
{
public:
    MediaCallBack();
    ~MediaCallBack();

    void setAudioFormatCallBack(FUN_SetAudioFormat audioFormat);
    void setAudioRenderData(FUN_SetAudioData audioData);
    void setVideoFormatCallBack(FUN_SetVideoFormat videoFormat);
    void createVideoFrameOver(FUN_CreateFrameFinish createFrameFinish);
    void getVideoFrameBuffer(FUN_GetVideoBuffer videoBuffer);
    void clearRendererCallBack(FUN_ClearRenderer renderer);
    void positionChangeCallBack(FUN_PositionChange positionChange);
    void openFinishTypeCallBack(FUN_OpenFinishType openFinishType);
    void playFinishCallBack(FUN_PlayFinish playFinish);
    void playStopCallBack(FUN_PlayStop playStop);
    void setIOCallBack(FUN_IO inputCallback);
    void setSeekCallBack(FUN_Seek seekCallback);


    void setAudioFormat(int &sampleRate, int &aduioChannels,int &aduioFormat);
    void setAudioData(const unsigned char* pAudioData, int nAudioSize);
    void setVideoFormat(int &nWidth,int &nHeight,int &nFormat);
    void createVideoFrameFinish();
    unsigned char* getVideoBuffer();
    void clearRenderer();
    void positionChange(int position);
    void openFinshType(int type);
    void playFinsh();
    void playStop();
    int64_t readData(unsigned char* buffer, int len);
    int64_t seekData(int64_t offset, int whence);

private:
    FUN_SetAudioData m_setAudioData;
    FUN_SetAudioFormat m_setAudioFormat;
    FUN_SetVideoFormat m_setVideoFormat;
    FUN_CreateFrameFinish m_createFrameFinish;
    FUN_GetVideoBuffer m_videoBuffer;
    FUN_ClearRenderer m_clearRenderer;
    FUN_PositionChange m_positionChange;
    FUN_OpenFinishType m_openFinishType;
    FUN_PlayFinish m_playFinish;
    FUN_PlayStop m_playStop;
    FUN_IO m_inputCallback;
    FUN_Seek m_seekCallback;

};

#endif // MEDIACALLBACK_H
