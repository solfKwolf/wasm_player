#ifndef ISCALE_H
#define ISCALE_H

#include <unknown.h>

extern "C" IID IID_ISCALE;

interface IScale : public IUNknown
{
    enum IScaleError
    {
        NoError = 0,
        InitError = 1,
        ParamError = 2,
        NotInit = 3,
        ConvertError = 4,
    };

    enum AudioSampleFormat
    {
        AudioFormateNone = -1,
        AudioFormateU8,          ///< unsigned 8 bits
        AudioFormateS16,         ///< signed 16 bits
        AudioFormateS32,         ///< signed 32 bits
        AudioFormateFLT,         ///< float
        AudioFormateDBL,         ///< double
        AudioFormateU8P,         ///< unsigned 8 bits, planar
        AudioFormateS16P,        ///< signed 16 bits, planar
        AudioFormateS32P,        ///< signed 32 bits, planar
        AudioFormateFLTP,        ///< float, planar
        AudioFormateDBLP,        ///< double, planar
        AudioFormateS64,         ///< signed 64 bits
        AudioFormateS64P,        ///< signed 64 bits, planar
    };

    enum FrameFormat
    {
        FormatNone = -1,
        FormatRGB24 = 0,
        FormatRGBA = 1,
        FormatBGRA = 2,
        FormatGRAY8 = 3,
        FormatYUV420P = 4,
        ForamtARGB = 5,
        ForamtNV12 = 6,
        ForamtNV21 = 7,
    };

    /**
     * @brief getPictureSize            获取相应图像格式/宽/高对应的数据大小
     * @param format                    图像格式
     * @param nWidth                    图像宽
     * @param nHeight                   图像高
     * @return                          图像所占大小
     */
    virtual int getPictureSize(FrameFormat format, int nWidth, int nHeight) = 0;

    /**
     * @brief convertImg                转换图像格式, 调用之前需要调用initVideoContext接口
     * @param frameUnknown              源Frame接口,转换成功原来格式改为转换后的.
     * @param outBuffer                 输出的buffer.如果为空,转换器内部申请;不为空,大小为getPictureSize接口返回的大小,需要外部申请和释放.
     * @return
     */
    virtual IScaleError convertImg(IUNknown *frameUnknown, unsigned char *outBuffer,
                                   FrameFormat outFormat, int outWidth, int outHeight) = 0;

    /**
     * @brief uninitVideoHandle         删除图像转换句柄(需要改变转换格式时调用)
     */
    virtual void uninitVideoHandle() = 0;

    /**
     * @brief converAudio               转换音频格式
     * @param frameUnknown              源Frame接口
     * @param nAudioChannels            音频通道数
     * @param nSampleRate               音频采样率
     * @param sampleFormat              音频数据格式
     * @return
     */
    virtual IScaleError converAudio(IUNknown *frameUnknown, int nAudioChannels, int nSampleRate, AudioSampleFormat sampleFormat) = 0;

    /**
     * @brief uninitAudioHandle         删除音频转换句柄(需要改变转换格式时调用)
     */
    virtual void uninitAudioHandle() = 0;

};

#endif // ISCALE_H
