#ifndef IDECODE_H
#define IDECODE_H

#include <unknown.h>
#include <vector>

extern "C" IID IID_IDECODE;

interface IDecode : public IUNknown
{
    enum IDecodeError
    {
        NoError = 0,
        InitError = 1,
        ParamError = 2,
        CodecOpenError = 3,
        DecodeEof = 4,
        DecodeError = 5,
        FrameUseless = 6,           // 解码器未出帧
        DecoderInvalid = 7,
    };

    /**
     * @brief initDecoder       初始化解码器
     * @param pIStream          初始化解码器的IStream接口
     * @return                  IDecodeError Code
     */
    virtual IDecodeError initDecoder(IUNknown* pIStream) = 0;

    /**
     * @brief decodeData        解码一包数据
     * @param pIPacket          放入解码器的数据(IPacket接口)
     * @param pIFrame           解码器解码成功的数据(IFrame接口)
     * @return                  IDecodeError Code
     */
    virtual IDecodeError decodeData(IUNknown* pIPacket, IUNknown* pIFrame) = 0;

    /**
     * @brief setHardware       设置解码器解码方式(硬解or软件)
     * @param needHardware
     */
    virtual void setHardware(bool needHardware) = 0;

    /**
     * @brief getHardware       获取解码器解码方式
     * @return
     */
    virtual bool getHardware() = 0;
};

#endif // IDECODE_H

