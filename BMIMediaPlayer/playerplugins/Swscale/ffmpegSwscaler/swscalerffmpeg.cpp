extern "C"
{
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
}
#include <stream.h>
#include <iostream>
#include <frame.h>
#include <map>
#include "swscalerffmpeg.h"

static std::map<IScale::AudioSampleFormat, AVSampleFormat> g_scaleAudioSampleFormatMap =
{
    {IScale::AudioFormateNone, AV_SAMPLE_FMT_NONE},
    {IScale::AudioFormateU8, AV_SAMPLE_FMT_U8},
    {IScale::AudioFormateS16, AV_SAMPLE_FMT_S16},
    {IScale::AudioFormateS32, AV_SAMPLE_FMT_S32},
    {IScale::AudioFormateFLT, AV_SAMPLE_FMT_FLT},
    {IScale::AudioFormateDBL, AV_SAMPLE_FMT_DBL},
    {IScale::AudioFormateU8P, AV_SAMPLE_FMT_U8P},
    {IScale::AudioFormateS16P, AV_SAMPLE_FMT_S16P},
    {IScale::AudioFormateS32P, AV_SAMPLE_FMT_S32P},
    {IScale::AudioFormateFLTP, AV_SAMPLE_FMT_FLTP},
    {IScale::AudioFormateDBLP, AV_SAMPLE_FMT_DBLP},
    {IScale::AudioFormateS64, AV_SAMPLE_FMT_S64},
    {IScale::AudioFormateS64P, AV_SAMPLE_FMT_S64P},
};

static std::map<IScale::FrameFormat, AVPixelFormat> g_sacleFrameFormatMap =
{
    {IScale::FormatNone, AV_PIX_FMT_NONE},
    {IScale::FormatRGB24, AV_PIX_FMT_RGB24},
    {IScale::FormatRGBA, AV_PIX_FMT_RGBA},
    {IScale::FormatBGRA, AV_PIX_FMT_BGRA},
    {IScale::FormatGRAY8, AV_PIX_FMT_GRAY8},
    {IScale::FormatYUV420P, AV_PIX_FMT_YUV420P},
    {IScale::ForamtARGB, AV_PIX_FMT_ARGB},
    {IScale::ForamtNV12, AV_PIX_FMT_NV12},
    {IScale::ForamtNV21, AV_PIX_FMT_NV21},
};

static std::map<IFrame::AudioSampleFormat, AVSampleFormat> g_frameAudioSampleFormatMap =
{
    {IFrame::AudioFormateNone, AV_SAMPLE_FMT_NONE},
    {IFrame::AudioFormateU8, AV_SAMPLE_FMT_U8},
    {IFrame::AudioFormateS16, AV_SAMPLE_FMT_S16},
    {IFrame::AudioFormateS32, AV_SAMPLE_FMT_S32},
    {IFrame::AudioFormateFLT, AV_SAMPLE_FMT_FLT},
    {IFrame::AudioFormateDBL, AV_SAMPLE_FMT_DBL},
    {IFrame::AudioFormateU8P, AV_SAMPLE_FMT_U8P},
    {IFrame::AudioFormateS16P, AV_SAMPLE_FMT_S16P},
    {IFrame::AudioFormateS32P, AV_SAMPLE_FMT_S32P},
    {IFrame::AudioFormateFLTP, AV_SAMPLE_FMT_FLTP},
    {IFrame::AudioFormateDBLP, AV_SAMPLE_FMT_DBLP},
    {IFrame::AudioFormateS64, AV_SAMPLE_FMT_S64},
    {IFrame::AudioFormateS64P, AV_SAMPLE_FMT_S64P},
};

HRESULT SwscalerFFmpeg::QueryInterface(const IID &iid, void **ppv)
{
    return m_pUnknowOuter->QueryInterface(iid, ppv);
}

ULONG SwscalerFFmpeg::AddRef()
{
    return m_pUnknowOuter->AddRef();
}

ULONG SwscalerFFmpeg::Release()
{
    return m_pUnknowOuter->Release();
}

HRESULT SwscalerFFmpeg::NondelegatingQueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUNknown)
    {
        *ppv = static_cast<INondelegatingUnknown*>(this);
    }
    else if(iid == IID_ISCALE)
    {
        *ppv = static_cast<IScale*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUNknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG SwscalerFFmpeg::NondelegatingAddRef()
{
    return ++m_cRef;
}

ULONG SwscalerFFmpeg::NondelegatingRelease()
{
    if(--m_cRef == 0)
    {
        std::cout << "COM Swscale FFmpeg delete." << std::endl;
        delete this;
        return 0;
    }
    return m_cRef;
}

SwscalerFFmpeg::SwscalerFFmpeg(IUNknown *pUnknowOuter)
    : m_cRef(0)
    , m_imgConvert(nullptr)
    , m_audioConvert(nullptr)
{
    if(pUnknowOuter)
    {
        // 被聚合的，使用外部unknown
        m_pUnknowOuter = pUnknowOuter;
    }
    else
    {
        // 没被聚合，使用非代理unknown
        m_pUnknowOuter = reinterpret_cast<IUNknown*>(static_cast<INondelegatingUnknown*>(this));
    }
}

SwscalerFFmpeg::~SwscalerFFmpeg()
{
    std::cout << "delete swscaler" << std::endl;
    if(m_imgConvert)
    {
        sws_freeContext(m_imgConvert);
        m_imgConvert = nullptr;
    }
    if(m_audioConvert)
    {
        swr_free(&m_audioConvert);
        m_audioConvert = nullptr;
    }
}

IScale::IScaleError SwscalerFFmpeg::initVideoContext(IUNknown *frameUnknown, IScale::FrameFormat outFormat, int outWidth, int outHeight)
{
    IPtr<IFrame,&IID_IFRAME> spIFrame(frameUnknown);
    if(!spIFrame || spIFrame->frameName() != "FFMPEG FRAME")
    {
        return ParamError;
    }

    enum AVPixelFormat outPix = (enum AVPixelFormat)convertFormat(outFormat);
    m_imgConvert = sws_getCachedContext(m_imgConvert, spIFrame->frameWidth(), spIFrame->frameHeight(), (enum AVPixelFormat)convertFormat(convertFormatFrame2Sacle(spIFrame->getFrameFormat())),
                                        outWidth, outHeight, outPix, SWS_BICUBIC, nullptr, nullptr, nullptr);
    if(m_imgConvert)
        return NoError;
    return InitError;
}

int SwscalerFFmpeg::getPictureSize(IScale::FrameFormat format, int nWidth, int nHeight)
{
    return av_image_get_buffer_size((enum AVPixelFormat)convertFormat(format) , nWidth, nHeight, 1);
}

IScale::IScaleError SwscalerFFmpeg::convertImg(IUNknown *frameUnknown, unsigned char *outBuffer,
                                               FrameFormat outFormat, int outWidth, int outHeight)
{
    IPtr<IFrame,&IID_IFRAME> spIFrame(frameUnknown);
    if(!spIFrame || spIFrame->frameName() != "FFMPEG FRAME")
    {
        return ParamError;
    }
    if(initVideoContext(frameUnknown, outFormat, outWidth, outHeight) != NoError)
        return InitError;

    std::function<void(void*)> releaseFunc;
    if(outBuffer)
    {
        releaseFunc = [&](void* pFrameData)
        {
            AVFrame* frame = (AVFrame*)pFrameData;
            av_frame_free(&frame);
        };
    }
    else
    {
        int nOutSize = getPictureSize(outFormat, outWidth, outHeight);
        outBuffer = (unsigned char*)av_malloc(nOutSize);
        releaseFunc = [&](void* pFrameData)
        {
            AVFrame* frame = (AVFrame*)pFrameData;
            if(frame->data[0])
                av_freep(&frame->data[0]);
            av_frame_free(&frame);
        };
    }

    auto frame = getFrame(frameUnknown);

    AVFrame *pFrameScale = av_frame_alloc();
    av_image_fill_arrays(pFrameScale->data, pFrameScale->linesize, (uint8_t*)outBuffer, (enum AVPixelFormat)convertFormat(outFormat), outWidth, outHeight, 1);
    sws_scale(m_imgConvert, frame->data, frame->linesize, 0, frame->height, pFrameScale->data, pFrameScale->linesize);

    spIFrame->setPrivateData(pFrameScale, releaseFunc);
    spIFrame->setFrameSize(outWidth, outHeight);
    int frameDataSize = getPictureSize(outFormat, outWidth, outHeight);
    spIFrame->setFrameData(pFrameScale->data[0], frameDataSize);
    return NoError;
}

void SwscalerFFmpeg::uninitVideoHandle()
{
    if(m_imgConvert)
    {
        sws_freeContext(m_imgConvert);
        m_imgConvert = nullptr;
    }
}

IScale::IScaleError SwscalerFFmpeg::converAudio(IUNknown *frameUnknown, int nAudioChannels, int nSampleRate, IScale::AudioSampleFormat sampleFormat)
{
    IPtr<IFrame,&IID_IFRAME> spIFrame(frameUnknown);
    if(!spIFrame || spIFrame->frameName() != "FFMPEG FRAME")
    {
        return ParamError;
    }

    if(!m_audioConvert)
    {
        IScale::IScaleError error = initAudioContext(frameUnknown, nAudioChannels, nSampleRate, sampleFormat);
        if(error != NoError)
            return error;
    }

    auto frame = getFrame(frameUnknown);

    AVFrame* outFrame = av_frame_alloc();
    // transSample
    int64_t srcSamplesNB = frame->nb_samples;
    outFrame->nb_samples = av_rescale_rnd(swr_get_delay(m_audioConvert, spIFrame->getSampleRate())+srcSamplesNB,
                                          spIFrame->getSampleRate(), spIFrame->getSampleRate(), AV_ROUND_UP);
    outFrame->channels = frame->channels;
    int ret = av_samples_alloc(outFrame->data, &outFrame->linesize[0], outFrame->channels, outFrame->nb_samples, (enum AVSampleFormat)converAudioFormat(sampleFormat),0);

    if(ret < 0)
    {
        av_frame_free(&outFrame);
        outFrame = nullptr;
        return ConvertError;
    }
    //输入也可能是分平面的，所以要做如下处理
    uint8_t* data[MAXCHANNELS];
    setupArray(data, frame.get(), converAudioFormat(spIFrame->getAudioSampleFormat()), frame->nb_samples);

    int length = swr_convert(m_audioConvert, outFrame->data, outFrame->nb_samples, (const uint8_t**)data, srcSamplesNB);
    if(length < 0)
    {
        av_frame_free(&outFrame);
        outFrame = nullptr;
        return ConvertError;
    }

    // 替换Frame数据
    auto releaseFunc = [&](void* pFrameData)
    {
        AVFrame* frame = (AVFrame*)pFrameData;
        if(frame->data[0])
            av_freep(&frame->data[0]);
        av_frame_free(&frame);
    };
    spIFrame->setPrivateData(outFrame, releaseFunc);
    spIFrame->setAudioChannels(nAudioChannels);
    spIFrame->setAudioSampleFormat((IFrame::AudioSampleFormat)sampleFormat);
    spIFrame->setSampleRate(nSampleRate);
    spIFrame->setFrameData(outFrame->data[0], outFrame->linesize[0]);
    return NoError;
}

void SwscalerFFmpeg::uninitAudioHandle()
{
    if(m_audioConvert)
    {
        swr_free(&m_audioConvert);
        m_audioConvert = nullptr;
    }
}

std::shared_ptr<AVFrame> SwscalerFFmpeg::getFrame(IUNknown *frameUnknown)
{
    AVFrame *frame = av_frame_alloc();
    IPtr<IFrame,&IID_IFRAME> spIFrame(frameUnknown);
    frame->channels = spIFrame->getAduioChannels();
    frame->nb_samples = spIFrame->getNBSamples();
    frame->height = spIFrame->frameHeight();
    frame->width = spIFrame->frameWidth();
    frame->format = convertFormat(convertFormatFrame2Sacle(spIFrame->getFrameFormat()));

    int len = 0;
    const unsigned char* data = spIFrame->getFrameData(len);
    if(spIFrame->getFrameType() == IFrame::AVTypeVideo)
    {
//        av_image_alloc(frame->data, frame->linesize, frame->width, frame->height, (AVPixelFormat)frame->format, 1);
        av_image_fill_arrays(frame->data, frame->linesize, (uint8_t *)data, (AVPixelFormat)frame->format, frame->width, frame->height, 1);
    }
    else if(spIFrame->getFrameType() == IFrame::AVTypeAudio)
    {
        frame->linesize[0] = len/frame->channels;
        for(int i = 0; i < frame->channels; i++)
        {
            frame->data[i] = (uint8_t*)(data+i*frame->linesize[0]);
        }
    }
    else
    {
        frame->linesize[0] = len;
        frame->data[0] = (uint8_t *)data;
    }
    auto retPtr = std::shared_ptr<AVFrame>(frame, [](AVFrame *pFrame){
            av_frame_free(&pFrame);
        });
    return std::move(retPtr);
}

int SwscalerFFmpeg::convertFormat(IScale::FrameFormat format)
{
    auto it = g_sacleFrameFormatMap.find(format);
    if(it != g_sacleFrameFormatMap.end())
        return it->second;
    return AV_PIX_FMT_NONE;
}

IScale::FrameFormat SwscalerFFmpeg::convertFormatFrame2Sacle(IFrame::FrameFormat format)
{
    return (IScale::FrameFormat)format;
}

int SwscalerFFmpeg::converAudioFormat(IScale::AudioSampleFormat format)
{
    auto it = g_scaleAudioSampleFormatMap.find(format);
    if(it != g_scaleAudioSampleFormatMap.end())
        return it->second;
    return AV_SAMPLE_FMT_NONE;
}

int SwscalerFFmpeg::converAudioFormat(IFrame::AudioSampleFormat format)
{
    auto it = g_frameAudioSampleFormatMap.find(format);
    if(it != g_frameAudioSampleFormatMap.end())
        return it->second;
    return AV_SAMPLE_FMT_NONE;
}

IScale::IScaleError SwscalerFFmpeg::initAudioContext(IUNknown *frameUnknown, int nAudioChannels, int nSampleRate, IScale::AudioSampleFormat sampleFormat)
{
    IPtr<IFrame,&IID_IFRAME> spIFrame(frameUnknown);
    if(!spIFrame || spIFrame->frameName() != "FFMPEG FRAME")
    {
        return ParamError;
    }

    if(m_audioConvert)
    {
       swr_free(&m_audioConvert);
       m_audioConvert = nullptr;
    }

    m_audioConvert = swr_alloc();
    m_audioConvert = swr_alloc_set_opts(m_audioConvert, av_get_default_channel_layout(nAudioChannels), (enum AVSampleFormat)converAudioFormat(sampleFormat), nSampleRate,
                                        av_get_default_channel_layout(spIFrame->getAduioChannels()), (enum AVSampleFormat)converAudioFormat(spIFrame->getAudioSampleFormat()), spIFrame->getSampleRate(),
                                        0, nullptr);
    swr_init(m_audioConvert);
    return NoError;
}

void SwscalerFFmpeg::setupArray(uint8_t *out[], AVFrame *inframe, int format, int samples)
{
    if (av_sample_fmt_is_planar((AVSampleFormat)format))
    {
        int i;
        format &= 0xFF;
        //从decoder出来的frame中的data数据不是连续分布的，所以不能这样写：in_frame->data[0]+i*planeSize;
        for (i = 0; i < inframe->channels; i++)
        {
            out[i] = inframe->data[i];
        }
    }
    else
    {
        out[0] = inframe->data[0];
    }
}

#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
IUNknown *CreateInstance(IUNknown *pUnknowOuter)
{
    std::cout <<"CreateInstance of SwscalerFFmpeg" << std::endl;
    IUNknown* pI = (IUNknown*)(void*)new SwscalerFFmpeg(pUnknowOuter);
    pI->AddRef();
    return pI;
}
#endif
