#ifndef IDEMUX_H
#define IDEMUX_H

#include <unknown.h>
#include <memory>
#include <functional>
#include <string>

extern "C" IID IID_IDEMUX;

typedef std::function<int64_t(unsigned char*, int)> IOCallBack;
typedef std::function<int64_t(int64_t, int)> SeekCallBack;

class IOReader {
public:
    virtual ~IOReader(){}
    virtual int64_t Read(unsigned char* data, unsigned long len) = 0;
    virtual int64_t Seek(int64_t pos, unsigned long whence) = 0;
};

interface IDemux : public IUNknown
{
    enum IDemuxError
    {
        NoError = 0,
        InitError = 1,
        ParamError = 2,
        DemuxerEof = 3,
        DemuxError = 4,
        NeedlessConver = 5,
        StreamIndexError = 6,
        NotInit = 7,
    };

    /**
     * @brief initDemuxer           初始化解复用器
     * @param reader
     * @return
     */
    virtual IDemuxError initDemuxer(IOReader *reader) {
        if(reader == nullptr)
            return ParamError;
        auto cbReadIO = std::bind(&IOReader::Read, reader, std::placeholders::_1, std::placeholders::_2);
        auto cbSeekIO = std::bind(&IOReader::Seek, reader, std::placeholders::_1, std::placeholders::_2);
        return initDemuxer(cbReadIO, cbSeekIO);
    }

    /**
     * @brief initDemuxer           初始化解复用器
     * @param url                   使用的url
     * @return                      IDemuxError Code
     */
    virtual IDemuxError initDemuxer(std::string url) = 0;

    /**
     * @brief initDemuxer           初始化解复用器
     * @param inputCallback         读取数据的回掉函数
     *        函数原型：int64_t(unsigned char* buffer, int len)
     *                @param buffer 读取数据使用的缓冲区首地址
     *                @param len    读取数据使用的缓冲区长度
     *                @return 读取到的数据长度
     *
     * @param seekCallback          跳转时的回掉函数
     *        函数原型：int64_t(int64_t offset, int whence)
     *                @param offset 跳转的偏移量
     *                @param whence 跳转的起始位置
     *                @return 跳转的位置
     *
     * @return IDemuxError Code
     */
    virtual IDemuxError initDemuxer(IOCallBack inputCallback, SeekCallBack seekCallback = nullptr) = 0;

    /**
     * @brief demuxStream           解复用一包数据
     * @param pUnknow               IPacket接口
     * @return
     */
    virtual IDemuxError demuxStream(IUNknown* pUnknow) = 0;

    /**
     * @brief converToNAL           解复用出来的数据可能不能直接保存文件，需要转换成完整的NAL
     * @param pDst                  目标Packet
     * @param pSrc                  源Packet
     * @return                      转换成功 - NoError 不需要转换 - NeedlessConver
     */
    virtual IDemuxError converToNAL(IUNknown* pDst, IUNknown* pSrc, bool addAACHeader = true) = 0;

    /**
     * @brief seek                  跳转
     * @param nSecond               跳转到相应的时间(秒)
     * @return                      true 跳转成功 false 跳转失败
     */
    virtual bool seek(int nSecond) = 0;

    /**
     * @brief setScale              设置播放速度(rtsp时有效)
     * @param scale                 播放速度
     */
    virtual void setScale(float scale) = 0;

    /**
     * @brief duration              视频长度
     * @return                      视频长度(秒)
     */
    virtual int64_t duration() = 0;

//    virtual void getAudioFormat(int& nSampleRate, int& nChannelCount, int& nSampleSize) = 0;

//    virtual void getVideoSize(int &nWidth, int &nHeight) = 0;

//    virtual void setVideoInfo(int nWidth, int nHeight, int nFormat) = 0;
//    virtual void getVideoInfo(int &nWidth, int &nHeight, int &nFormat) = 0;

    /**
     * @brief pause 暂停(网络流时需要调用)
     */
    virtual void pause() = 0;

    /**
     * @brief resume 重新开始(网络流暂停)
     */
    virtual void resume() = 0;

    /**
     * @brief getStreamCount        获取流数量
     * @return                      流数量
     */
    virtual int getStreamCount() = 0;

    /**
     * @brief getStream             获取对应索引的IStream接口
     * @param nStreamIndex          流索引
     * @param pIStream              返回的IStream接口
     * @return
     */
    virtual IDemuxError getStream(int nStreamIndex, IUNknown* pIStream) = 0;
};

#endif // IDEMUX_H
