extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
}
#include "encoderffmpeg.h"
#include <iostream>
#include <stream.h>
#include <frame.h>
#include <packet.h>
#include <map>

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

HRESULT EncoderFFmpeg::QueryInterface(const IID &iid, void **ppv)
{
    return m_pUnknowOuter->QueryInterface(iid, ppv);
}

ULONG EncoderFFmpeg::AddRef()
{
    return m_pUnknowOuter->AddRef();
}

ULONG EncoderFFmpeg::Release()
{
    return m_pUnknowOuter->Release();
}

HRESULT EncoderFFmpeg::NondelegatingQueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUNknown)
    {
        *ppv = static_cast<INondelegatingUnknown*>(this);
    }
    else if(iid == IID_IENCODE)
    {
        *ppv = static_cast<IEncode*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUNknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG EncoderFFmpeg::NondelegatingAddRef()
{
    return ++m_cRef;
}

ULONG EncoderFFmpeg::NondelegatingRelease()
{
    if(--m_cRef == 0)
    {
        std::cout << "COM Encoder FFmpeg delete." << std::endl;
        delete this;
        return 0;
    }
    return m_cRef;
}

EncoderFFmpeg::EncoderFFmpeg(IUNknown *pUnknowOuter)
    : m_cRef(0)
    , m_nBitrate(-1)
    , m_nGopsize(-1)
    , m_nFramerate(-1)
    , m_nMaxQuality(-1)
    , m_nMinQuality(-1)
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

    m_pPacket.reset(av_packet_alloc(), [&](AVPacket *pPacket){
        av_packet_free(&pPacket);
    });
}

EncoderFFmpeg::~EncoderFFmpeg()
{
    m_pCodecContext.reset();
    m_pPacket.reset();
    std::cout << "delete encoder" << std::endl;
}

IEncode::IEncodeError EncoderFFmpeg::initEncoder(IUNknown *pIStream, std::string strEncoderName)
{
    IPtr<IAVStream, &IID_ISTREAM> spIStream(pIStream);
    if(!spIStream)
        return ParamError;

    if(spIStream->streamName() != "FFMPEG STREAM")
        return ParamError;

    AVCodecParameters *codecParameter = avcodec_parameters_alloc();
    codecParFFmpegMeida2FFmpeg(codecParameter, pIStream);

    if(codecParameter->codec_type != AVMEDIA_TYPE_VIDEO)
    {
        return MediaTypeError;
    }

    AVCodec *pEncoder = nullptr;
    if(!strEncoderName.empty())
    {
        pEncoder = avcodec_find_encoder_by_name(strEncoderName.c_str());
    }

    if(pEncoder == nullptr)
    {
        pEncoder = avcodec_find_encoder(codecParameter->codec_id);
    }

    if(pEncoder == nullptr)
    {
        return NotFoundEncoder;
    }

    m_pCodecContext.reset(avcodec_alloc_context3(pEncoder), [](AVCodecContext *pContext){
        avcodec_close(pContext);
        avcodec_free_context(&pContext);
    });

    auto timebase = spIStream->getTimebase();
    m_pCodecContext->time_base = AVRational{timebase.num, timebase.den};
    avcodec_parameters_to_context(m_pCodecContext.get(), codecParameter);

    if(m_nBitrate != -1)
    {
        m_pCodecContext->bit_rate = m_nBitrate;
    }

    if(m_nGopsize != -1)
    {
        m_pCodecContext->gop_size = m_nGopsize;
    }

    int tmpQuality = 0;
    if(m_nMinQuality != -1 && m_nMaxQuality != -1)
        tmpQuality = (m_nMinQuality + m_nMaxQuality)/2;
    else if(m_nMinQuality != -1)
        tmpQuality = m_nMinQuality;
    else if(m_nMaxQuality != -1)
        tmpQuality = m_nMaxQuality;

    if(tmpQuality != 0)
        m_pCodecContext->global_quality = tmpQuality;

    m_pCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if(avcodec_open2(m_pCodecContext.get(), pEncoder, NULL) < 0) {
        return OpenEncoderError;
    }

    spIStream->setBitrate(m_pCodecContext->global_quality);
    if(m_pCodecContext->extradata != nullptr)
        spIStream->setCodecExtradata(std::string((char*)m_pCodecContext->extradata, m_pCodecContext->extradata_size));

    return NoError;
}

IEncode::IEncodeError EncoderFFmpeg::encodeData(IUNknown *pIFrame, IUNknown *pIPacket)
{
    IPtr<IFrame, &IID_IFRAME> spIFrame(pIFrame);
    if(!spIFrame)
    {
        return ParamError;
    }
    IPtr<IPacket, &IID_IPACKET> spIPacket(pIPacket);
    if(!spIPacket)
    {
        return ParamError;
    }

    std::shared_ptr<AVFrame> frame;
    if(!spIFrame->isEnd())
    {
        frame = getFrame(pIFrame);
    }
    else
    {
        frame.reset(av_frame_alloc(), [](AVFrame *pFrame){
            av_frame_free(&pFrame);
        });
    }
    int ret = avcodec_send_frame(m_pCodecContext.get(), frame.get());
    if(ret == AVERROR(EINVAL) || ret == AVERROR(ENOMEM))
        return EncodeError;

    av_init_packet(m_pPacket.get());
    ret = avcodec_receive_packet(m_pCodecContext.get(), m_pPacket.get());
    if(spIFrame->isEnd() && ret == AVERROR_EOF)
    {
        spIPacket->setEofFlag(true);
        spIPacket->setStreamIndex(spIFrame->getStreamIndex());
        return EncoderEof;
    }
    else if(ret == AVERROR(EAGAIN))
    {
        return FrameUseless;
    }
    else if(ret < 0)
        return EncodeError;

    AVPacket* outPacket = av_packet_alloc();
    av_packet_move_ref(outPacket, m_pPacket.get());
    auto releaseFunc = [&](void* packet)
    {
        AVPacket* pkt = (AVPacket*)packet;
        av_packet_free(&pkt);
    };

    IPacket::AVMediaType mediaType = IPacket::AVTypeUnknow;
    switch (m_pCodecContext->codec_type) {
    case AVMEDIA_TYPE_UNKNOWN:
        mediaType = IPacket::AVTypeUnknow;
        break;
    case AVMEDIA_TYPE_VIDEO:
        mediaType = IPacket::AVTypeVideo;
        break;
    case AVMEDIA_TYPE_AUDIO:
        mediaType = IPacket::AVTypeAudio;
        break;
    case AVMEDIA_TYPE_DATA:
        mediaType = IPacket::AVTypeData;
        break;
    case AVMEDIA_TYPE_SUBTITLE:
        mediaType = IPacket::AVTypeSubtitle;
        break;
    case AVMEDIA_TYPE_ATTACHMENT:
        mediaType = IPacket::AVTypeAttachment;
        break;
    default:
        break;
    }

    spIPacket->setPrivateData(outPacket, releaseFunc);
    spIPacket->setPts(spIFrame->getPts());
    spIPacket->setDts(spIFrame->getPts());
    spIPacket->setPacketData(outPacket->data, outPacket->size);
    spIPacket->setKeyframeFlag(outPacket->flags | AV_PKT_FLAG_KEY);
    spIPacket->setStreamIndex(outPacket->stream_index);
    spIPacket->setPacketType(mediaType);
    spIPacket->setPacketDuration(outPacket->duration);
    spIPacket->setPacketPos(outPacket->pos);
    spIPacket->setPacketFlag(outPacket->flags);
    return NoError;
}

void EncoderFFmpeg::setBitrate(int64_t bitrate)
{
    m_nBitrate = bitrate;
}

int64_t EncoderFFmpeg::getBitrate()
{
    return m_nBitrate;
}

void EncoderFFmpeg::setGopSize(int gopSize)
{
    m_nGopsize = gopSize;
}

int EncoderFFmpeg::getGopSize()
{
    return m_nGopsize;
}

void EncoderFFmpeg::setFramerate(int framerate)
{
    m_nFramerate = framerate;
}

int EncoderFFmpeg::getFramerate()
{
    return m_nFramerate;
}

void EncoderFFmpeg::setMaxQuality(int quality)
{
    m_nMaxQuality = quality;
}

int EncoderFFmpeg::getMaxQuality()
{
    return m_nMinQuality;
}

void EncoderFFmpeg::setMinQuality(int quality)
{
    m_nMinQuality = quality;
}

int EncoderFFmpeg::getMinQuality()
{
    return m_nMinQuality;
}

void EncoderFFmpeg::setEncodeType(IEncode::IEncodeTpye type)
{

}

IEncode::IEncodeTpye EncoderFFmpeg::getEncodeType()
{
    return EncodeTypeCBR;
}

void EncoderFFmpeg::codecParFFmpegMeida2FFmpeg(AVCodecParameters *par, IUNknown *pIStream)
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

std::shared_ptr<AVFrame> EncoderFFmpeg::getFrame(IUNknown *frameUnknown)
{
    AVFrame *frame = av_frame_alloc();
    IPtr<IFrame,&IID_IFRAME> spIFrame(frameUnknown);
    frame->channels = spIFrame->getAduioChannels();
    frame->nb_samples = spIFrame->getNBSamples();
    frame->height = spIFrame->frameHeight();
    frame->width = spIFrame->frameWidth();
    frame->format = convertFormat(spIFrame->getFrameFormat());

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

int EncoderFFmpeg::convertFormat(IFrame::FrameFormat format)
{
    enum AVPixelFormat pix = AV_PIX_FMT_NONE;
    switch (format) {
    case IFrame::FormatRGB24:
        pix = AV_PIX_FMT_RGB24;
        break;
    case IFrame::FormatRGBA:
        pix = AV_PIX_FMT_RGBA;
        break;
    case IFrame::FormatBGRA:
        pix = AV_PIX_FMT_BGRA;
        break;
    case IFrame::FormatGRAY8:
        pix = AV_PIX_FMT_GRAY8;
        break;
    case IFrame::FormatYUV420P:
        pix = AV_PIX_FMT_YUV420P;
        break;
    case IFrame::ForamtARGB:
        pix = AV_PIX_FMT_ARGB;
        break;
    case IFrame::ForamtNV12:
        pix = AV_PIX_FMT_NV12;
        break;
    case IFrame::ForamtNV21:
        pix = AV_PIX_FMT_NV21;
        break;
    default:
        break;
    }
    return pix;
}

#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
IUNknown *CreateInstance(IUNknown *pUnknowOuter)
{
    std::cout <<"CreateInstance of EncoderFFmpeg" << std::endl;
    IUNknown* pI = (IUNknown*)(void*)new EncoderFFmpeg(pUnknowOuter);
    pI->AddRef();
    return pI;
}
#endif
