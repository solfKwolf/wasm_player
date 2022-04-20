#ifndef IMUX_H
#define IMUX_H

#include <unknown.h>
#include <memory>
#include <functional>
#include <vector>
#include <string>

extern "C" IID IID_IMUX;

typedef std::function<int64_t(unsigned char*, int)> IOCallBack;

class IOWriter {
public:
    virtual ~IOWriter(){}
    virtual int64_t Write(unsigned char *data, unsigned long len) = 0;
};

interface IMux : public IUNknown
{
    enum IMuxError
    {
        NoError = 0,
        ParamError = 1,
        InitError = 2,
        OpenURLError = 3,
        WriteHeaderError = 4,
        CreateStreamError = 5,
        MuxError = 6,
    };

    virtual IMuxError initMuxer(IOWriter *writer, std::string format) {
        if(writer == nullptr)
            return ParamError;
        auto writerIO = std::bind(&IOWriter::Write, writer, std::placeholders::_1, std::placeholders::_2);
        return initMuxer(writerIO, format);
    }

    /**
     * @brief initMuxer         初始化复用器
     * @param url               文件/网络流地址
     * @return
     */
    virtual IMuxError initMuxer(std::string url, std::string format) = 0;

    /**
     * @brief initMuxer         初始化复用器
     * @param outputCallback    输出的回调函数
     * @return
     */
    virtual IMuxError initMuxer(IOCallBack outputCallback, std::string format) = 0;

    /**
     * @brief addStream         增加一个输出流
     * @param pUnknow           IStream接口 -- video: codectype/streamtype/framesize/extradata -- sps,pps
     *                                        audio: codectype/streamtype/sample rate/channel count
     * @return
     */
    virtual IMuxError addStream(IUNknown *pUnknow) = 0;

    /**
     * @brief writeHeader       增加Stream完成之后，写文件头
     * @return
     */
    virtual IMuxError writeHeader() = 0;

    /**
     * @brief writeTrailer      复用完成之后，写文件尾
     * @return
     */
    virtual IMuxError writeTrailer() = 0;

    /**
     * @brief muxPacket         复用一包数据
     * @param pUnknow           IPacket接口
     * @return
     */
    virtual IMuxError muxPacket(IUNknown *pUnknow) = 0;

    virtual std::vector<std::string> getAllCodecString() = 0;
};

#endif // IMUX_H
