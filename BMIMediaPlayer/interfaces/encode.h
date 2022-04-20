#ifndef IENCODE_H
#define IENCODE_H

#include <unknown.h>
#include <string>

extern "C" IID IID_IENCODE;

interface IEncode : public IUNknown
{
    enum IEncodeError
    {
        NoError = 0,
        InitError = 1,
        ParamError = 2,
        MediaTypeError = 3,
        NotFoundEncoder = 4,
        OpenEncoderError = 5,
        EncodeError = 6,
        EncoderEof = 7,
        FrameUseless = 8,
    };

    enum IEncodeTpye
    {
        EncodeTypeCBR,
        EncodeTypeVBR,
        EncodeTypeFixQP,
    };

    /**
     * @brief initEncoder           初始化编码器, 可能更新IStream接口内容
     * @param pIStream              IStream接口，携带编码器信息
     * @param strEncoderName        指定使用的编码器名称，不指定为默认编码器
     * @return                      ErrorCode
     */
    virtual IEncodeError initEncoder(IUNknown* pIStream, std::string strEncoderName = "") = 0;

    /**
     * @brief encodeData            编码一帧数据
     * @param pIFrame               输入的帧数据
     * @param pIPacket              输出的包数据
     * @return                      ErrorCode
     */
    virtual IEncodeError encodeData(IUNknown* pIFrame, IUNknown* pIPacket) = 0;

    /**
     * @brief setBitrate            设置输出比特率
     * @param bitrate               比特率
     */
    virtual void setBitrate(int64_t bitrate) = 0;

    /**
     * @brief getBitrate            获取输出比特率
     * @return                      比特率
     */
    virtual int64_t getBitrate() = 0;

    /**
     * @brief setGopSize            设置I帧间隔
     * @param gopSize               一个gop分组多少帧
     */
    virtual void setGopSize(int gopSize) = 0;

    /**
     * @brief getGopSize            获取I帧间隔
     * @return                      一个gop分组多少帧
     */
    virtual int getGopSize() = 0;

    /**
     * @brief setFramerate          设置输出帧率
     * @param framerate             输出帧率
     */
    virtual void setFramerate(int framerate) = 0;

    /**
     * @brief getFramerate          获取输出帧率
     * @return                      输出帧率
     */
    virtual int getFramerate() = 0;

    /**
     * @brief setMaxQuality         设置最大q值
     * @param quality               Qp
     */
    virtual void setMaxQuality(int quality) = 0;

    /**
     * @brief getMaxQuality         获取最大q值
     * @return                      Qp
     */
    virtual int getMaxQuality() = 0;

    /**
     * @brief setMinQuality         设置最小q值
     * @param quality               Qp
     */
    virtual void setMinQuality(int quality) = 0;

    /**
     * @brief getMinQuality         获取最小q值
     * @return                      Qp
     */
    virtual int getMinQuality() = 0;

    /**
     * @brief setEncodeType         设置编码类型
     * @param type                  编码类型
     */
    virtual void setEncodeType(IEncodeTpye type) = 0;

    /**
     * @brief getEncodeType         获取编码类型
     * @return                      编码类型
     */
    virtual IEncodeTpye getEncodeType() = 0;
};

#endif // IENCODE_H

