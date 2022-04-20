#ifndef IPLAY_H
#define IPLAY_H

#include <unknown.h>
#include <functional>

extern "C" IID IID_IPLAY;

typedef std::function<int64_t(unsigned char*, int)> FUN_IO;
typedef std::function<int64_t(int64_t, int)> FUN_Seek;
typedef std::function<void (int&,int&,int&)> FUN_SetAudioFormat;
typedef std::function<void (const unsigned char*, int)> FUN_SetAudioData;
typedef std::function<void (int&,int&,int&)> FUN_SetVideoFormat;
typedef std::function<void (void)> FUN_CreateFrameFinish;
typedef std::function<unsigned char* (void)> FUN_GetVideoBuffer;
typedef std::function<void (void)> FUN_ClearRenderer;
typedef std::function<void (int)> FUN_PositionChange;
typedef std::function<void (int)> FUN_OpenFinishType;
typedef std::function<void (void)> FUN_PlayFinish;
typedef std::function<void (void)> FUN_PlayStop;


interface IPlay : public IUNknown
{
    enum ClockType
    {
        VideoClock,
        AudioClock,
        OtherClock,
        NoneClock
    };

    virtual void Play() = 0;

    virtual void PlayFile(const char* fileName) = 0;

    virtual void PlayStream(bool isStream) = 0;

    virtual void Stop() = 0;

    virtual void Pause() = 0;

    virtual void Resume() = 0;

    virtual void TogglePause() = 0;

    virtual int64_t Duration() = 0;

    virtual bool Seek(int percent) = 0;

    virtual bool isRunning() = 0;

    virtual float volume() = 0;

    virtual void setVolume(float volume) = 0;

    virtual void setScale(float scale) = 0;

    virtual float getScale() = 0;

    virtual void setSpeed(float fSpeed) = 0;

    virtual float getSpeed() = 0;

    virtual void autoSpeed(bool autoSpeed = true) = 0;

    virtual int64_t getCoderate() = 0;

    virtual void setCodeRateSecond(int nSecond) = 0;

    virtual void setHardware(bool needHardware) = 0;

    virtual bool getHardware() = 0;

    virtual void setClockType(IPlay::ClockType type) = 0;

    virtual void setCacheTime(int nSecond) = 0;

    virtual void setMaxAutoSpeed(float nSpeed) = 0;

    virtual void setAudioRenderFormat(FUN_SetAudioFormat audioFormat) = 0;
    virtual void setAudioRenderData(FUN_SetAudioData audioData) = 0;
    virtual void setVideoRenderFormat(FUN_SetVideoFormat audioFormat) = 0 ;
    virtual void createVideoFrameOver(FUN_CreateFrameFinish createFrameFinish) = 0;
    virtual void getVideoFrameBuffer(FUN_GetVideoBuffer videoBuffer) = 0;
    virtual void clearRenderer(FUN_ClearRenderer renderer) = 0;
    virtual void positionChange(FUN_PositionChange positionChange) = 0;
    virtual void openFinishType(FUN_OpenFinishType openFinish) = 0;
    virtual void playFinish(FUN_PlayFinish playFinish) = 0;
    virtual void playStop(FUN_PlayStop playStop) = 0;
    virtual void setIOCallBack(FUN_IO inputCallback) = 0;
    virtual void setSeekCallBack(FUN_Seek seekCallback) = 0;


    /**
     * @brief SetFileName
     * @param fileName
     */
    virtual void SetFileName(const char* fileName) = 0;
    virtual char* GetFileName() = 0;

    virtual void setQRCodeText(const char* strText) = 0;

};

#endif // IPLAY_H

