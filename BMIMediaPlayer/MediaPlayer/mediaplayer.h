#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#ifdef __WIN32__
#if defined(MEDIAPLAYER_LIBRARY)
#  define MEDIAPLAYERSHARED_EXPORT __declspec(dllexport)
#else
#  define MEDIAPLAYERSHARED_EXPORT __declspec(dllimport)
#endif
#else
#  define MEDIAPLAYERSHARED_EXPORT
#endif

#include <atomic>
#include <unknown.h>
#include <play.h>

class MediaFunction;

class MediaPlayer
        : public INondelegatingUnknown
        , public IPlay
{
public:
    virtual HRESULT __stdcall QueryInterface(const IID &iid, void **ppv);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();

    virtual HRESULT __stdcall NondelegatingQueryInterface(const IID& iid, void **ppv);
    virtual ULONG __stdcall NondelegatingAddRef();
    virtual ULONG __stdcall NondelegatingRelease();

    MediaPlayer(IUNknown *pUnknowOuter);
    virtual ~MediaPlayer();

    virtual void Play() override;
    virtual void PlayFile(const char* fileName) override;
    virtual void PlayStream(bool isStream) override;
    virtual void Stop() override;
    virtual void Pause() override;
    virtual void Resume() override;
    virtual void TogglePause() override;
    virtual bool Seek(int percent) override;
    virtual int64_t Duration() override;
    virtual void SetFileName(const char* fileName);
    virtual char* GetFileName() ;
    virtual bool isRunning() override;
    virtual float volume() override;
    virtual void setVolume(float volume) override;
    virtual void setScale(float scale) override;
    virtual float getScale() override;
    virtual void setSpeed(float fSpeed) override;
    virtual float getSpeed() override;
    virtual void autoSpeed(bool autoSpeed = true) override;
    virtual int64_t getCoderate() override;
    virtual void setHardware(bool needHardware) override;
    virtual bool getHardware() override;
    virtual void setCodeRateSecond(int nSecond) override;
    virtual void setClockType(IPlay::ClockType type) override;
    virtual void setCacheTime(int nSecond) override;
    virtual void setMaxAutoSpeed(float nSpeed) override;

    virtual void setAudioRenderFormat(FUN_SetAudioFormat audioFormat) override;
    virtual void setAudioRenderData(FUN_SetAudioData audioData) override;
    virtual void setVideoRenderFormat(FUN_SetVideoFormat videoFormat) override;
    virtual void createVideoFrameOver(FUN_CreateFrameFinish createFrameFinish) override;
    virtual void getVideoFrameBuffer(FUN_GetVideoBuffer videoBuffer) override;
    virtual void clearRenderer(FUN_ClearRenderer renderer) override;
    virtual void positionChange(FUN_PositionChange positionChange) override;
    virtual void openFinishType(FUN_OpenFinishType openFinish) override;
    virtual void playFinish(FUN_PlayFinish playFinish) override;
    virtual void playStop(FUN_PlayStop playStop) override;
    virtual void setIOCallBack(FUN_IO inputCallback) override;
    virtual void setSeekCallBack(FUN_Seek seekCallback) override;

    virtual void setQRCodeText(const char* strText) override;
private:
    /**
     * @brief 组件实例的引用计数
     */
    std::atomic_ulong m_cRef;

    /**
     * @brief 指向非代理Unknown接口或外部组件Unknown接口
     */
    IUNknown* m_pUnknowOuter;

    MediaFunction *m_pMediaFunc;
};


extern "C"
{
#ifdef __WIN32__
MEDIAPLAYERSHARED_EXPORT
#else
#endif // __WIN32__
IUNknown *CreateInstance(IUNknown* pUnknowOuter = nullptr);
}
#endif // MEDIAPLAYER_H
