#ifndef MEDIADATA_H
#define MEDIADATA_H
#include <mutex>
#include <string>

class MediaData
{
    MediaData();
    class MediaDataPrivate
    {
    public:
        ~MediaDataPrivate()
        {
            if(MediaData::m_pInstance)
                delete MediaData::m_pInstance;
        }
    };
public:
    enum PlayStatus
    {
        Pause,
        Play,
        Normal
    };

    ~MediaData();
    static MediaData* getInstance();

    void setFilename(std::string filename);
    std::string getFilename();

    void setStreamCount(int count);
    int getStreamCount();

    void clearStreamIndex();
    void setVStreamIndex(int index);
    int getVStramIndex();

    void setAStreamIndex(int index);
    int getAStramIndex();

    void setSStreamIndex(int index);
    int getSStramIndex();

    void addReleaseIndex();
    void resetReleaseIndex();
    int getReleaseIndex();

    void setAutoSpeed(bool autoSpeed);
    bool getAutoSpeed();

    void setSpeed(float speed);
    float getSpeed();

    void setScale(float scale);
    float getScale();

    void setVolume(float volume);
    float getVolume();

    void setAudioFormat(int nSampleRate, int nChannelCount, int nSampleSize);
    void getAudioFormat(int &nSampleRate, int &nChannelCount, int &nSampleSize);

    void setVideoFormat(int nWidth, int nHeight, int nFormat);
    void getVideoFormat(int &nWidth,int &nHeight,int &nFormat);

    void setHardware(bool needHardware);
    bool getHardware();

    void setLowDelay(bool bLowDelay);

    void setIODevice(bool isIODevice);
    bool getIODevice();

    void setMaxFramelistSize(int framelistsize);
    int getMaxFramelistSize();

    void setMaxPacketlistSize(int packetlistSize);
    int getMaxPacketlistSize();

    void setFrameRate(int nFrameRate);
    int getFrameRate();

    void isEnableCache(bool enabled);
    void setCacheTime(int nSecond);
    int getCacheTime();

    PlayStatus judgePlayStatus(int nVideoSize, int nAudioSize);

private:
    static MediaDataPrivate m_private;
    static MediaData* m_pInstance;
    static std::mutex m_mutex;

    std::string m_FileName;

    int m_nStreamCount;
    int m_nVStreamIndex;
    int m_nAStreamIndex;
    int m_nSStreamIndex;
    int m_releaseIndex;

    int m_nSampleRate;
    int m_nChannelCount;
    int m_nSampleFormat;
    int m_nVideoWidth;
    int m_nVideoHeight;
    int m_nPixelFormat;

    float m_fVolume;
    float m_fScale;
    float m_fSpeed;
    bool m_autoSpeed;
    bool m_needHardWare;
    bool m_isIODevice;

    int m_maxFramelistSize;
    int m_maxPacketlistSize;

    int m_nFrameRate;
    int m_nCacheTime;

    bool m_isEnableCache;
    bool m_isRepeat;

};

#endif // MEDIADATA_H
