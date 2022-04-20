extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
}
#include "demuxerffmpeg.h"
#include <iostream>
#include <packet.h>
#include <stream.h>
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

static std::map<IPacket::AVMediaType, AVMediaType> g_packetMediaTypeMap =
{
    {IPacket::AVTypeUnknow, AVMEDIA_TYPE_UNKNOWN},
    {IPacket::AVTypeVideo, AVMEDIA_TYPE_VIDEO},
    {IPacket::AVTypeAudio, AVMEDIA_TYPE_AUDIO},
    {IPacket::AVTypeData, AVMEDIA_TYPE_DATA},
    {IPacket::AVTypeSubtitle, AVMEDIA_TYPE_SUBTITLE},
    {IPacket::AVTypeAttachment, AVMEDIA_TYPE_ATTACHMENT},
};

static int readPacker(void *user, uint8_t *buffer, int size)
{
    DemuxerFFmpeg* obj = (DemuxerFFmpeg*) user;
    if(obj)
        return obj->readData(buffer, size);
    return 0;
}

static int64_t seekPacket(void *user, int64_t offset, int whence)
{
    DemuxerFFmpeg* obj = (DemuxerFFmpeg*) user;
    if(obj)
        return obj->seekData(offset, whence);
    return 0;
}

HRESULT DemuxerFFmpeg::QueryInterface(const IID &iid, void **ppv)
{
    return m_pUnknowOuter->QueryInterface(iid, ppv);
}

ULONG DemuxerFFmpeg::AddRef()
{
    return m_pUnknowOuter->AddRef();
}

ULONG DemuxerFFmpeg::Release()
{
    return m_pUnknowOuter->Release();
}

HRESULT DemuxerFFmpeg::NondelegatingQueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUNknown)
    {
        *ppv = static_cast<INondelegatingUnknown*>(this);
    }
    else if(iid == IID_IDEMUX)
    {
        *ppv = static_cast<IDemux*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUNknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG DemuxerFFmpeg::NondelegatingAddRef()
{
    return ++m_cRef;
}

ULONG DemuxerFFmpeg::NondelegatingRelease()
{
    if(--m_cRef == 0)
    {
        std::cout << "COM Demuxer FFmpeg delete." << std::endl;
        delete this;
        return 0;
    }
    return m_cRef;
}

DemuxerFFmpeg::DemuxerFFmpeg(IUNknown *pUnknowOuter)
    : m_cRef(0)
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

DemuxerFFmpeg::~DemuxerFFmpeg()
{
    std::cout << "delete demxuer" << std::endl;
    clearDemuxer();
    m_bsfContext.reset();
    m_hevcBsfContext.reset();
    m_aacBsfContext.reset();
}

IDemux::IDemuxError DemuxerFFmpeg::initDemuxer(std::string url)
{
    clearDemuxer();

    AVFormatContext *pOutputContext = nullptr;
    AVDictionary *inputOpts = nullptr;
    av_dict_set(&inputOpts, "rtsp_transport", "tcp", 0);
    av_dict_set(&inputOpts, "stimeout", "20000000", 0);
    int nRet = avformat_open_input(&pOutputContext, url.c_str(), nullptr, &inputOpts);
    av_dict_free(&inputOpts);
    if(nRet < 0)
    {
        return InitError;
    }
    m_pInputContext.reset(pOutputContext, [&](AVFormatContext *pOutputContext){
        avformat_close_input(&pOutputContext);
        avformat_free_context(pOutputContext);
    });
    if(avformat_find_stream_info(m_pInputContext.get(), 0) < 0)
        return InitError;
    return NoError;
}

IDemux::IDemuxError DemuxerFFmpeg::initDemuxer(IOCallBack inputCallback, SeekCallBack seekCallback)
{
    if(inputCallback == nullptr)
        return InitError;

    clearDemuxer();

    m_pWriteBuffer.reset((unsigned char*)av_malloc(WriteBufferSize), [&](unsigned char* data){
        if(m_pIOContext && m_pIOContext->buffer == data)
            av_free(data);
    });

    m_inputCallback = inputCallback;
    m_seekCallBack = seekCallback;

    AVIOContext *pIOContext = avio_alloc_context(m_pWriteBuffer.get(), WriteBufferSize, 0, (void*)this, readPacker, nullptr, seekPacket);
    if(!pIOContext)
        return InitError;

    m_pIOContext.reset(pIOContext, [&](AVIOContext *ioContext){
        av_free(ioContext);
    });

    m_pInputContext.reset(avformat_alloc_context(), [&](AVFormatContext *pInputContext) {
        avformat_close_input(&pInputContext);
        avformat_free_context(pInputContext);
    });

    if(!m_pInputContext)
    {
        clearDemuxer();
        return InitError;
    }

    AVInputFormat* pInputFormat = nullptr;
    if(av_probe_input_buffer(pIOContext, &pInputFormat, "", nullptr, 0, 0) < 0)
    {
        clearDemuxer();
        return InitError;
    }
    std::cout <<"format" << pInputFormat->name << pInputFormat->long_name << std::endl;

    AVDictionary *inputOpts = nullptr;
    av_dict_set(&inputOpts, "rtsp_transport", "tcp", 0);
    av_dict_set(&inputOpts, "stimeout", "20000000", 0);

    m_pInputContext->pb = pIOContext;
    auto tmpInputContext = m_pInputContext.get();
    int nRet = avformat_open_input(&tmpInputContext,nullptr, pInputFormat, &inputOpts);
    av_dict_free(&inputOpts);
    if(nRet < 0)
    {
        clearDemuxer();
        return InitError;
    }
    if(avformat_find_stream_info(m_pInputContext.get(), 0) < 0)
    {
        clearDemuxer();
        return InitError;
    }
    return NoError;
}

IDemux::IDemuxError DemuxerFFmpeg::demuxStream(IUNknown *pUnknow)
{
    IPtr<IPacket, &IID_IPACKET> spIPacket(pUnknow);
    if(!spIPacket)
        return ParamError;

    AVPacket* packet = av_packet_alloc();

    int ret = av_read_frame(m_pInputContext.get(), packet);
    if(ret < 0)
    {
        av_packet_free(&packet);
        if(ret == AVERROR_EOF || avio_feof(m_pInputContext->pb))
        {
            spIPacket->setEofFlag(true);
            return DemuxerEof;
        }
        else if (m_pInputContext->pb && m_pInputContext->pb->error)
            return DemuxError;
        else
            return DemuxError;
    }

    if(packet->size == 0)
    {
        av_packet_free(&packet);
        return DemuxError;
    }

    auto releaseFunc = [&](void* packet)
    {
        AVPacket* pkt = (AVPacket*)packet;
        av_packet_free(&pkt);
    };

    AVStream *pStream = m_pInputContext->streams[packet->stream_index];
    IPacket::AVMediaType mediaType = IPacket::AVTypeUnknow;

    for (auto it = g_packetMediaTypeMap.begin(); it != g_packetMediaTypeMap.end(); it++)
    {
        if(it->second == pStream->codecpar->codec_type)
        {
            mediaType = it->first;
            break;
        }
    }

    spIPacket->setPrivateData(packet, releaseFunc);
    spIPacket->setPts(packet->pts * av_q2d(pStream->time_base) * 1000);
    spIPacket->setDts(packet->dts * av_q2d(pStream->time_base) * 1000);
    spIPacket->setPacketData(packet->data, packet->size);
    spIPacket->setKeyframeFlag(packet->flags & AV_PKT_FLAG_KEY);
    spIPacket->setStreamIndex(packet->stream_index);
    spIPacket->setPacketType(mediaType);
    spIPacket->setPacketDuration(packet->duration);
    spIPacket->setPacketPos(packet->pos);
    spIPacket->setPacketFlag(packet->flags);
    return NoError;
}

IDemux::IDemuxError DemuxerFFmpeg::converToNAL(IUNknown* pDst, IUNknown* pSrc, bool addAACHeader)
{
    IPtr<IPacket, &IID_IPACKET> spSrc(pSrc);
    IPtr<IPacket, &IID_IPACKET> spDst(pDst);
    if(!spSrc || !spDst)
        return ParamError;
    if(spSrc->packetName() != "FFMPEG PACKET" || spDst->packetName() != "FFMPEG PACKET")
        return ParamError;

    if(spSrc->getStreamIndex() >= (int)m_pInputContext->nb_streams)
        return StreamIndexError;

    int nSrcLen = 0;
    const unsigned char* pSrcData = spSrc->getPacketData(nSrcLen);
    if(pSrcData == nullptr)
        return ParamError;

    AVBSFContext *pBsfContext = nullptr;
    AVStream *inStream = m_pInputContext->streams[spSrc->getStreamIndex()];
    switch (inStream->codecpar->codec_id) {
    case AV_CODEC_ID_H264:
    {
        if(!m_bsfContext)
        {
            m_bsfContext.reset(createBSFContext("h264_mp4toannexb", inStream), [&](AVBSFContext* bsfContex){
                av_bsf_free(&bsfContex);
            });
        }
        pBsfContext = m_bsfContext.get();
    }
        break;
    case AV_CODEC_ID_HEVC:
        if(!m_hevcBsfContext)
        {
            m_hevcBsfContext.reset(createBSFContext("hevc_mp4toannexb", inStream), [&](AVBSFContext* bsfContex){
                av_bsf_free(&bsfContex);
            });
        }
        pBsfContext = m_hevcBsfContext.get();
        break;
    case AV_CODEC_ID_AAC:
        if(!m_aacBsfContext)
        {
            m_aacBsfContext.reset(createBSFContext("aac_adtstoasc", inStream), [&](AVBSFContext* bsfContex){
                av_bsf_free(&bsfContex);
            });
        }
        pBsfContext = m_aacBsfContext.get();
        break;
    default:
        pBsfContext = nullptr;
        break;
    }

    unsigned char* pDstData = nullptr;
    int nDstDataSize = 0;
    std::function<void(void*)> releaseFunc;
    void* pPrivateData = nullptr;

    if(pBsfContext == nullptr || pSrcData == 0)
    {
        return NeedlessConver;
    }
    else if(pBsfContext == m_aacBsfContext.get() && addAACHeader == true)
    {
        unsigned char* aacPacket = (unsigned char*)av_malloc(nSrcLen + 7);
        addAACHead(aacPacket, inStream->codecpar, nSrcLen);
        memcpy(aacPacket+7, pSrcData, nSrcLen);
        releaseFunc = [&](void* packet)
        {
            av_freep(&packet);
        };
        pDstData = aacPacket;
        nDstDataSize = nSrcLen+7;
        pPrivateData = aacPacket;
    }
    else
    {
        AVPacket* inPacket = av_packet_alloc();
        getPacket(inPacket, pSrc);

        int ret = av_bsf_send_packet(pBsfContext, inPacket);
        if(ret < 0)
        {
            av_packet_free(&inPacket);
            return DemuxError;
        }
        AVPacket* packet = av_packet_alloc();
        ret = av_bsf_receive_packet(pBsfContext, packet);
        if(ret < 0)
        {
            av_packet_free(&packet);
            av_packet_free(&inPacket);
            return DemuxError;
        }

        releaseFunc = [&](void* packet)
        {
            AVPacket* pkt = (AVPacket*)packet;
            av_packet_free(&pkt);
        };
        pDstData = packet->data;
        nDstDataSize = packet->size;
        pPrivateData = packet;
        av_packet_free(&inPacket);
    }

    spDst->setPrivateData(pPrivateData, releaseFunc);
    spDst->setPacketData(pDstData, nDstDataSize);
    spDst->setPts(spSrc->getPts());
    spDst->setDts(spSrc->getDts());
    spDst->setKeyframeFlag(spSrc->isKeyframe());
    spDst->setStreamIndex(spSrc->getStreamIndex());
    spDst->setEofFlag(spSrc->isEnd());
    spDst->setPacketType(spDst->getPacketType());
    return NoError;
}

bool DemuxerFFmpeg::seek(int nSecond)
{
    int defaultStreamIndex = av_find_default_stream_index(m_pInputContext.get());
    if(defaultStreamIndex >= (int)m_pInputContext->nb_streams)
        defaultStreamIndex = 0;
    if (m_pInputContext)
    {
        auto time_base = m_pInputContext->streams[defaultStreamIndex]->time_base;
        auto startTime = m_pInputContext->streams[defaultStreamIndex]->start_time * av_q2d(time_base);
        auto seekTime = startTime + nSecond;
        auto seekTimestamp = AV_TIME_BASE * seekTime;

        if (av_seek_frame(m_pInputContext.get(), -1, seekTimestamp, AVSEEK_FLAG_BACKWARD) < 0)
        {
            return false;
        }
        return true;
    }
    return false;
}

void DemuxerFFmpeg::getPacket(AVPacket *pkt, IUNknown *pIPacket)
{
    if(pkt == nullptr)
        return;
    IPtr<IPacket, &IID_IPACKET> spIPacket(pIPacket);
    AVRational timebase = m_pInputContext->streams[spIPacket->getStreamIndex()]->time_base;
    pkt->dts = spIPacket->getDts() * timebase.den / timebase.num /1000.0;
    pkt->pts = spIPacket->getPts() * timebase.den / timebase.num /1000.0;
    pkt->pos = spIPacket->getPacketPos();
    pkt->flags = spIPacket->getPacketFlag();
    pkt->duration = spIPacket->getPacketDuration();
    pkt->stream_index = spIPacket->getStreamIndex();
    int dataLen = 0;
    const unsigned char* pData = spIPacket->getPacketData(dataLen);

    if(dataLen > 0)
    {
        pkt->data = (uint8_t *)pData;
        pkt->size = dataLen;
    }
}

AVBSFContext *DemuxerFFmpeg::createBSFContext(std::string bsfName, AVStream *inStream)
{
    AVBSFContext* bsfContex = nullptr;
    const AVBitStreamFilter *filer = av_bsf_get_by_name(bsfName.c_str());
    if(filer)
    {
        av_bsf_alloc(filer, &bsfContex);
    }
    avcodec_parameters_copy(bsfContex->par_in, inStream->codecpar);
    bsfContex->time_base_in = inStream->time_base;
    av_bsf_init(bsfContex);
    return bsfContex;
}

void DemuxerFFmpeg::setScale(float scale)
{
    if(m_pInputContext)
    {
        av_dict_set(&m_pInputContext->metadata,"Speed",std::to_string(scale).c_str(),0);
        resume();
    }
}

int64_t DemuxerFFmpeg::duration()
{
    if(m_pInputContext)
    {
        if(m_pInputContext->duration == AV_NOPTS_VALUE)
        {
            return 0;
        }
        return m_pInputContext->duration/AV_TIME_BASE;
    }
    return 0;
}

void DemuxerFFmpeg::pause()
{
    if(m_pInputContext)
    {
        av_read_pause(m_pInputContext.get());
    }
}

void DemuxerFFmpeg::resume()
{
    if(m_pInputContext)
    {
        av_read_play(m_pInputContext.get());
    }
}

int DemuxerFFmpeg::getStreamCount()
{
    if(m_pInputContext)
    {
        return m_pInputContext->nb_streams;
    }
    return 0;
}

IDemux::IDemuxError DemuxerFFmpeg::getStream(int nStreamIndex, IUNknown *pIStream)
{
    if(!m_pInputContext)
        return NotInit;
    if((int)m_pInputContext->nb_streams <= nStreamIndex)
        return StreamIndexError;
    IPtr<IAVStream, &IID_ISTREAM> spStream(pIStream);
    if(!spStream)
        return ParamError;

    AVStream *pStream = m_pInputContext->streams[nStreamIndex];
    IAVStream::AVMediaType mediaType = IAVStream::AVTypeUnknow;
    for (auto it = g_streamMediaTypeMap.begin(); it != g_streamMediaTypeMap.end(); it++)
    {
        if(it->second == pStream->codecpar->codec_type)
        {
            mediaType = it->first;
            break;
        }
    }

    IAVStream::AVCodecType codecType = IAVStream::CodecTypeUnkow;
    for (auto it = g_streamCodecTypeMap.begin(); it != g_streamCodecTypeMap.end(); it++)
    {
        if(it->second == pStream->codecpar->codec_id)
        {
            codecType = it->first;
            break;
        }
    }

    spStream->setStreamCodecType(codecType);
    spStream->setStreamType(mediaType);
    spStream->setDuration(pStream->duration * av_q2d(pStream->time_base));
    spStream->setStreamIndex(pStream->index);

    if(mediaType == IAVStream::AVTypeVideo)
    {
        spStream->setFrameSize(pStream->codecpar->width, pStream->codecpar->height);
        IAVStream::FrameFormat frameFormat = IAVStream::FormatNone;
        for (auto it = g_streamFrameFormatMap.begin(); it != g_streamFrameFormatMap.end(); it++)
        {
            if(it->second == pStream->codecpar->format)
            {
                frameFormat = it->first;
                break;
            }
        }
        spStream->setFrameFormat(frameFormat);
    }
    else if (mediaType == IAVStream::AVTypeAudio)
    {
        IAVStream::AudioSampleFormat sampleType = IAVStream::AudioFormateNone;
        for (auto it = g_streamAudioSampleFormatMap.begin(); it != g_streamAudioSampleFormatMap.end(); it++)
        {
            if(it->second == pStream->codecpar->format)
            {
                sampleType = it->first;
                break;
            }
        }

        int nSampleRate = pStream->codecpar->sample_rate;
        int nChannels = pStream->codecpar->channels;
        spStream->setAudioFormat(nSampleRate, nChannels, sampleType);
    }
    IAVStream::StreamRational timebase;
    timebase.num = pStream->time_base.num;
    timebase.den = pStream->time_base.den;
    spStream->setTimebase(timebase);

    IAVStream::StreamRational frameRate;
    frameRate.num = pStream->r_frame_rate.num;
    frameRate.den = pStream->r_frame_rate.den;
    spStream->setFrameRate(frameRate);

    if(pStream->metadata != nullptr)
    {
        AVDictionaryEntry* title = nullptr;
        title = av_dict_get(pStream->metadata, "title", title, AV_DICT_IGNORE_SUFFIX);
        if(title && title->value)
            spStream->setStreamTitle(title->value);
    }

    codecParFFmpeg2Media(pStream->codecpar, pIStream);
    return NoError;
}

int64_t DemuxerFFmpeg::readData(unsigned char *buffer, int size)
{
    if(m_inputCallback)
        return m_inputCallback(buffer, size);
    return 0;
}

int64_t DemuxerFFmpeg::seekData(int64_t offset, int whence)
{
    if(m_seekCallBack)
        return m_seekCallBack(offset, whence);
    return 0;
}

void DemuxerFFmpeg::codecParFFmpeg2Media(AVCodecParameters *par, IUNknown *pIStream)
{
    IPtr<IAVStream, &IID_ISTREAM> spStream(pIStream);
    if(!spStream)
        return;
    spStream->setBitrate(par->bit_rate);
    if(par->extradata != nullptr)
        spStream->setCodecExtradata(std::string((char*)par->extradata, par->extradata_size));
}

void DemuxerFFmpeg::clearDemuxer()
{
    m_pWriteBuffer.reset();
    m_pIOContext.reset();
    m_pInputContext.reset();
}

void DemuxerFFmpeg::addAACHead(unsigned char *pData, AVCodecParameters *pContext, int packetSize)
{
    char bits[7] = {0};
    int sample_index = 0 , channel = 0;
    int length = 7 + packetSize;
    AVCodecParameters *audioCodecCtx = pContext;

    int high = (audioCodecCtx->extradata[0] & 0x07) << 1;
    int low = (audioCodecCtx->extradata[1]&0x80);
//    switch(audioCodecCtx->sample_rate)
//    {
//    case 96000:
//        sample_index = 0x0;
//        break;
//    case 88200:
//        sample_index = 0x1;
//        break;
//    case 64000:
//        sample_index = 0x2;
//        break;
//    case 48000:
//        sample_index = 0x3;
//        break;
//    case 44100:
//        sample_index = 0x4;
//        break;
//    case 32000:
//        sample_index = 0x5;
//        break;
//    case 24000:
//        sample_index = 0x6;
//        break;
//    case 22050:
//        sample_index = 0x7;
//        break;
//    case 16000:
//        sample_index = 0x8;
//        break;
//    case 12000:
//        sample_index = 0x9;
//        break;
//    case 11025:
//        sample_index = 0xa;
//        break;
//    case 8000:
//        sample_index = 0xb;
//        break;
//    case 7350:
//        sample_index = 0xc;
//        break;
//    default:
//        sample_index = sample_index + (temp>>7);
//        break;
//    }
    sample_index = high + (low>>7);
    channel = ((audioCodecCtx->extradata[1] - low) & 0xff) >> 3;
    bits[0] = 0xff;
    bits[1] = 0xf1;
    bits[2] = 0x40 | (sample_index<<2) | (channel>>2);
    bits[3] = ((channel&0x3)<<6) | (length >>11);
    bits[4] = (length>>3) & 0xff;
    bits[5] = ((length<<5) & 0xff) | 0x1f;
    memcpy(pData, bits, 7);
}

#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
IUNknown *CreateInstance(IUNknown *pUnknowOuter)
{
    std::cout <<"CreateInstance of DemuxerFFmpeg" << std::endl;
    IUNknown* pI = (IUNknown*)(void*)new DemuxerFFmpeg(pUnknowOuter);
    pI->AddRef();
    return pI;
}
#endif
