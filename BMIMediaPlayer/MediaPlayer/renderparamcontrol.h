#ifndef RENDERPARAMCONTROL_H
#define RENDERPARAMCONTROL_H

#include <mutex>
#include <play.h>

class RenderParamControl
{
    RenderParamControl();
    class RenderParamControlPrivate
    {
    public:
        ~RenderParamControlPrivate()
        {
            if(RenderParamControl::m_pInstance)
                delete RenderParamControl::m_pInstance;
        }
    };
public:
    ~RenderParamControl();
    static RenderParamControl* getInstance();

    void setStreamCount(int streamCount);
    int getStreamCount();
    void addSeekIndex();
    void resetSeekIndex();
    bool isSeekRender();
    void autoSpeed(bool autoSpeed);
    void autoSpeed(int nQueueMax, int nQueueSize);
    bool isAutoSpeed();
    int64_t getCodeRate();
    void setCodeRate(int nCodeRate);
    void setCodeRateSecond(int nSecond);
    int getCodeRateSecond();
    void setClockType(IPlay::ClockType type);
    void setMaxAutoSpeed(float nSpeed);
    IPlay::ClockType getClockType();

private:
    enum SpeedModel
    {
        SpeedUp,
        SpeedDown,
        SpeedNormal
    };
    static RenderParamControlPrivate m_private;
    static RenderParamControl* m_pInstance;
    static std::mutex m_mutex;
    int m_seekIndex;
    int m_nStreamCount;
    SpeedModel m_speedModel;
    bool m_autoSpeedFlag;
    float m_oldSpeed;
    float m_multiSpeed;
    int64_t m_nVideoCodeRate;
    int m_nSecond;

    IPlay::ClockType m_clockType;

};

#endif // RENDERPARAMCONTROL_H
