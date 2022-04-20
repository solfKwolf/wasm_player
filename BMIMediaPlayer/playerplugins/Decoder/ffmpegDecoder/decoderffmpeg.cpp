extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}
#include <stream.h>
#include <iostream>
#include <packet.h>
#include <frame.h>
#include <map>
#include "decoderffmpeg.h"
#include "realdecoder.h"

static std::map<IAVStream::AVMediaType, AVMediaType> g_streamMediaTypeMap =
{
    {IAVStream::AVTypeUnknow, AVMEDIA_TYPE_UNKNOWN},
    {IAVStream::AVTypeVideo, AVMEDIA_TYPE_VIDEO},
    {IAVStream::AVTypeAudio, AVMEDIA_TYPE_AUDIO},
    {IAVStream::AVTypeData, AVMEDIA_TYPE_DATA},
    {IAVStream::AVTypeSubtitle, AVMEDIA_TYPE_SUBTITLE},
    {IAVStream::AVTypeAttachment, AVMEDIA_TYPE_ATTACHMENT},
};

static std::map<IAVStream::AVCodecType, AVCodecID> g_streamCodecTypeMap =
{
    {IAVStream::CodecTypeUnkow, AV_CODEC_ID_NONE},
    {IAVStream::CodecTypeH264, AV_CODEC_ID_H264},
    {IAVStream::CodecTypeHevc, AV_CODEC_ID_HEVC},
    {IAVStream::CodecTypeAAC, AV_CODEC_ID_AAC},
    {IAVStream::CodecTypePCMALAW, AV_CODEC_ID_PCM_ALAW},
    {IAVStream::CodecTypePCMMULAM, AV_CODEC_ID_PCM_MULAW},
};

static std::map<IAVStream::AudioSampleFormat, AVSampleFormat> g_streamAudioSampleFormatMap =
{
    {IAVStream::AudioFormateNone, AV_SAMPLE_FMT_NONE},
    {IAVStream::AudioFormateU8, AV_SAMPLE_FMT_U8},
    {IAVStream::AudioFormateS16, AV_SAMPLE_FMT_S16},
    {IAVStream::AudioFormateS32, AV_SAMPLE_FMT_S32},
    {IAVStream::AudioFormateFLT, AV_SAMPLE_FMT_FLT},
    {IAVStream::AudioFormateDBL, AV_SAMPLE_FMT_DBL},
    {IAVStream::AudioFormateU8P, AV_SAMPLE_FMT_U8P},
    {IAVStream::AudioFormateS16P, AV_SAMPLE_FMT_S16P},
    {IAVStream::AudioFormateS32P, AV_SAMPLE_FMT_S32P},
    {IAVStream::AudioFormateFLTP, AV_SAMPLE_FMT_FLTP},
    {IAVStream::AudioFormateDBLP, AV_SAMPLE_FMT_DBLP},
    {IAVStream::AudioFormateS64, AV_SAMPLE_FMT_S64},
    {IAVStream::AudioFormateS64P, AV_SAMPLE_FMT_S64P},
};

static std::map<IAVStream::FrameFormat, AVPixelFormat> g_streamFrameFormatMap =
{
    {IAVStream::FormatNone, AV_PIX_FMT_NONE},
    {IAVStream::FormatRGB24, AV_PIX_FMT_RGB24},
    {IAVStream::FormatRGBA, AV_PIX_FMT_RGBA},
    {IAVStream::FormatBGRA, AV_PIX_FMT_BGRA},
    {IAVStream::FormatGRAY8, AV_PIX_FMT_GRAY8},
    {IAVStream::FormatYUV420P, AV_PIX_FMT_YUV420P},
    {IAVStream::ForamtARGB, AV_PIX_FMT_ARGB},
    {IAVStream::ForamtNV12, AV_PIX_FMT_NV12},
    {IAVStream::ForamtNV21, AV_PIX_FMT_NV21},
};

HRESULT DecoderFFmpeg::QueryInterface(const IID &iid, void **ppv)
{
    return m_pUnknowOuter->QueryInterface(iid, ppv);
}

ULONG DecoderFFmpeg::AddRef()
{
    return m_pUnknowOuter->AddRef();
}

ULONG DecoderFFmpeg::Release()
{
    return m_pUnknowOuter->Release();
}

HRESULT DecoderFFmpeg::NondelegatingQueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUNknown)
    {
        *ppv = static_cast<INondelegatingUnknown*>(this);
    }
    else if(iid == IID_IDECODE)
    {
        *ppv = static_cast<IDecode*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUNknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG DecoderFFmpeg::NondelegatingAddRef()
{
    return ++m_cRef;
}

ULONG DecoderFFmpeg::NondelegatingRelease()
{
    if(--m_cRef == 0)
    {
        std::cout << "COM Decoder FFmpeg delete." << std::endl;
        delete this;
        return 0;
    }
    return m_cRef;
}

DecoderFFmpeg::DecoderFFmpeg(IUNknown *pUnknowOuter)
    : m_cRef(0)
    , m_decoder(nullptr)
    , m_needHardWare(false)
{
    av_register_all();
    //Network
    avformat_network_init();
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

DecoderFFmpeg::~DecoderFFmpeg()
{
    std::cout << "delete decoder" << std::endl;
    if(m_decoder)
        delete m_decoder;
}

IDecode::IDecodeError DecoderFFmpeg::initDecoder(IUNknown *pIStream)
{
    IPtr<IAVStream, &IID_ISTREAM> spIStream(pIStream);
    if(!spIStream)
        return ParamError;

    if(spIStream->streamName() != "FFMPEG STREAM")
        return ParamError;

    // 获取流中的解码上下文
    AVCodecContext *pAvCodecCtx = nullptr;
    int ret = openCodec(m_needHardWare, &pAvCodecCtx, pIStream);
    if(ret < 0)
    {
        std::cout << "open decoder error..." << std::endl;
        ret = openCodec(false, &pAvCodecCtx, pIStream);
    }

    if(ret < 0)
        return CodecOpenError;

    // 包大小为0时可以丢弃
    m_decoder = RealDecoder::CreateDecoder(pAvCodecCtx, pIStream);
    if(m_decoder == nullptr)
        return DecoderInvalid;
    return NoError;
}

IDecode::IDecodeError DecoderFFmpeg::decodeData(IUNknown *pIPacket, IUNknown *pIFrame)
{
    if(m_decoder == nullptr)
        return DecoderInvalid;
    return m_decoder->decodeData(pIPacket, pIFrame);
}

bool DecoderFFmpeg::getHardware()
{
    return m_needHardWare;
}

void DecoderFFmpeg::setHardware(bool needHardware)
{
    m_needHardWare = needHardware;
}

bool DecoderFFmpeg::initCodec(bool needHardware, AVCodecContext **ppAvCodecCtx, AVCodecParameters *codecParameter)
{
    AVCodec *pCodec = nullptr;
    if(needHardware)
    {
        // 记录查找到的解码器
        switch (codecParameter->codec_id) {
#ifdef __WIN32__
        case AV_CODEC_ID_H264:
            pCodec = avcodec_find_decoder_by_name("h264_cuvid");
            break;
        case AV_CODEC_ID_HEVC:
            pCodec = avcodec_find_decoder_by_name("hevc_cuvid");
            break;
        case AV_CODEC_ID_MJPEG:
            pCodec = avcodec_find_decoder_by_name("mjpeg_cuvid");
            break;
        case AV_CODEC_ID_MPEG1VIDEO:
            pCodec = avcodec_find_decoder_by_name("mpeg1_cuvid");
            break;
        case AV_CODEC_ID_MPEG2VIDEO:
            pCodec = avcodec_find_decoder_by_name("mpeg2_cuvid");
            break;
        case AV_CODEC_ID_MPEG4:
            pCodec = avcodec_find_decoder_by_name("mpeg4_cuvid");
            break;
        case AV_CODEC_ID_VC1:
            pCodec = avcodec_find_decoder_by_name("vc1_cuvid");
            break;
        case AV_CODEC_ID_VP8:
            pCodec = avcodec_find_decoder_by_name("vp8_cuvid");
            break;
        case AV_CODEC_ID_VP9:
            pCodec = avcodec_find_decoder_by_name("vp9_cuvid");
            break;
#else
        case AV_CODEC_ID_H264:
            pCodec = avcodec_find_decoder_by_name("h264_mediacodec");
            break;
        case AV_CODEC_ID_HEVC:
            pCodec = avcodec_find_decoder_by_name("hevc_mediacodec");
            break;
        case AV_CODEC_ID_MPEG2VIDEO:
            pCodec = avcodec_find_decoder_by_name("mpeg2_mediacodec");
            break;
        case AV_CODEC_ID_MPEG4:
            pCodec = avcodec_find_decoder_by_name("mpeg4_mediacodec");
            break;
        case AV_CODEC_ID_VP8:
            pCodec = avcodec_find_decoder_by_name("vp8_mediacodec");
            break;
        case AV_CODEC_ID_VP9:
            pCodec = avcodec_find_decoder_by_name("vp9_mediacodec");
            break;
#endif
        default:
            break;
        }
    }
    if(pCodec == nullptr)
    {
        pCodec = avcodec_find_decoder(codecParameter->codec_id);
    }

    if(pCodec)
        std::cout << "codec name: " << pCodec->name << std::endl;
    else
        return false;

    *ppAvCodecCtx = avcodec_alloc_context3(pCodec);
    AVCodecContext *pAvCodecCtx = *ppAvCodecCtx;
    avcodec_parameters_to_context(pAvCodecCtx, codecParameter);

    // 设置分辨率,ffplay中nStreamLowres为传入参数
    int nStreamLowres = 0;
    // 设置加速加速,ffplay中nFast为传入参数
    int nFast = 0;

    if (nStreamLowres == 0)
        nStreamLowres = av_codec_get_max_lowres(pCodec);
    if(nStreamLowres > av_codec_get_max_lowres(pCodec))
        nStreamLowres = av_codec_get_max_lowres(pCodec);
    av_codec_set_lowres(pAvCodecCtx, nStreamLowres);

    //根据版本不同，设置不同的标志
#if FF_API_EMU_EDGE
    if(nStreamLowres) pAvCodecCtx->flags |= CODEC_FLAG_EMU_EDGE;
#endif

    if(nFast)
        pAvCodecCtx->flags2 |= AV_CODEC_FLAG2_FAST;

#if FF_API_EMU_EDGE
    if(pCodec->capabilities & AV_CODEC_CAP_DR1)
        pAvCodecCtx->flags |= CODEC_FLAG_EMU_EDGE;
#endif
    return true;
}

void DecoderFFmpeg::codecParFFmpegMeida2FFmpeg(AVCodecParameters *par, IUNknown *pIStream)
{
    IPtr<IAVStream, &IID_ISTREAM> spStream(pIStream);
    if(!spStream || !par)
        return;

    AVMediaType mediaType = AVMEDIA_TYPE_UNKNOWN;
    auto mediaIt = g_streamMediaTypeMap.find(spStream->getStreamType());
    if(mediaIt != g_streamMediaTypeMap.end())
        mediaType = mediaIt->second;
    par->codec_type = mediaType;

    AVCodecID codecType = AV_CODEC_ID_NONE;
    auto codecIt = g_streamCodecTypeMap.find(spStream->getStreamCodecType());
    if (codecIt != g_streamCodecTypeMap.end())
        codecType = codecIt->second;
    par->codec_id = codecType;

    if(spStream->getStreamType() == IAVStream::AVTypeVideo)
    {
        int width, height;
        spStream->getFrameSize(width, height);
        par->width                  = width;
        par->height                 = height;

        AVPixelFormat frameFormat = AV_PIX_FMT_NONE;
        auto mediaIt = g_streamFrameFormatMap.find(spStream->getFrameFormat());
        if(mediaIt != g_streamFrameFormatMap.end())
            frameFormat = mediaIt->second;
        par->format = frameFormat;
    }
    else if(spStream->getStreamType() == IAVStream::AVTypeAudio)
    {
        int nSamepleRate, channels;
        IAVStream::AudioSampleFormat audioFormat;
        spStream->getAudioFormat(nSamepleRate, channels, audioFormat);
        par->channels               = channels;
        par->sample_rate            = nSamepleRate;

        AVSampleFormat audioSampleFormat = AV_SAMPLE_FMT_NONE;
        auto sampleFormatIt = g_streamAudioSampleFormatMap.find(audioFormat);
        if(sampleFormatIt != g_streamAudioSampleFormatMap.end())
            audioSampleFormat = sampleFormatIt->second;
        par->format = audioSampleFormat;

    }

    par->bit_rate = spStream->getBitrate();
    std::string extradata = spStream->getCodecExtradata();
    if(extradata.empty())
    {
        par->extradata = nullptr;
        par->extradata_size = 0;
    }
    else
    {
        par->extradata = (uint8_t*)av_mallocz(extradata.size() + AV_INPUT_BUFFER_PADDING_SIZE);
        memcpy(par->extradata, extradata.c_str(), extradata.size());
        par->extradata_size = extradata.size();
    }
}

int DecoderFFmpeg::openCodec(bool needHardware, AVCodecContext **ppAvCodecCtx, IUNknown *pIStream)
{
    AVCodecParameters *codecParameter = avcodec_parameters_alloc();
    codecParFFmpegMeida2FFmpeg(codecParameter, pIStream);

    // 打开解码器
    if(codecParameter->codec_type == AVMEDIA_TYPE_VIDEO && (codecParameter->height <= 512 && codecParameter->width <= 512))
        initCodec(false, ppAvCodecCtx, codecParameter);
    else
        initCodec(needHardware, ppAvCodecCtx, codecParameter);
    avcodec_parameters_free(&codecParameter);

    IPtr<IAVStream, &IID_ISTREAM> spStream(pIStream);
    if(!spStream)
        return -1;
    AVRational timebase{spStream->getTimebase().num, spStream->getTimebase().den};
    av_codec_set_pkt_timebase(*ppAvCodecCtx, timebase);
    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "threads", "auto", 0);
    int ret = avcodec_open2(*ppAvCodecCtx, nullptr, &opts);
    av_dict_free(&opts);
    return ret;
}


#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
IUNknown *CreateInstance(IUNknown *pUnknowOuter)
{
    std::cout <<"CreateInstance of DecoderFFmpeg" << std::endl;
    IUNknown* pI = (IUNknown*)(void*)new DecoderFFmpeg(pUnknowOuter);
    pI->AddRef();
    return pI;
}
#endif
