extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/intreadwrite.h"
}
#include "muxerffmpeg.h"
#include <iostream>
#include <packet.h>
#include <map>

const std::map<AVCodecID, int> g_mp4ObjectType = {
    { AV_CODEC_ID_MOV_TEXT    , 0x08 },
    { AV_CODEC_ID_MPEG4       , 0x20 },
    { AV_CODEC_ID_H264        , 0x21 },
    { AV_CODEC_ID_HEVC        , 0x23 },
    { AV_CODEC_ID_AAC         , 0x40 },
    { AV_CODEC_ID_MP4ALS      , 0x40 }, /* 14496-3 ALS */
    { AV_CODEC_ID_MPEG2VIDEO  , 0x61 }, /* MPEG-2 Main */
    { AV_CODEC_ID_MPEG2VIDEO  , 0x60 }, /* MPEG-2 Simple */
    { AV_CODEC_ID_MPEG2VIDEO  , 0x62 }, /* MPEG-2 SNR */
    { AV_CODEC_ID_MPEG2VIDEO  , 0x63 }, /* MPEG-2 Spatial */
    { AV_CODEC_ID_MPEG2VIDEO  , 0x64 }, /* MPEG-2 High */
    { AV_CODEC_ID_MPEG2VIDEO  , 0x65 }, /* MPEG-2 422 */
    { AV_CODEC_ID_AAC         , 0x66 }, /* MPEG-2 AAC Main */
    { AV_CODEC_ID_AAC         , 0x67 }, /* MPEG-2 AAC Low */
    { AV_CODEC_ID_AAC         , 0x68 }, /* MPEG-2 AAC SSR */
    { AV_CODEC_ID_MP3         , 0x69 }, /* 13818-3 */
    { AV_CODEC_ID_MP2         , 0x69 }, /* 11172-3 */
    { AV_CODEC_ID_MPEG1VIDEO  , 0x6A }, /* 11172-2 */
    { AV_CODEC_ID_MP3         , 0x6B }, /* 11172-3 */
    { AV_CODEC_ID_MJPEG       , 0x6C }, /* 10918-1 */
    { AV_CODEC_ID_PNG         , 0x6D },
    { AV_CODEC_ID_JPEG2000    , 0x6E }, /* 15444-1 */
    { AV_CODEC_ID_VC1         , 0xA3 },
    { AV_CODEC_ID_DIRAC       , 0xA4 },
    { AV_CODEC_ID_AC3         , 0xA5 },
    { AV_CODEC_ID_EAC3        , 0xA6 },
    { AV_CODEC_ID_DTS         , 0xA9 }, /* mp4ra.org */
    { AV_CODEC_ID_OPUS        , 0xAD }, /* mp4ra.org */
    { AV_CODEC_ID_VP9         , 0xB1 }, /* mp4ra.org */
    { AV_CODEC_ID_FLAC        , 0xC1 }, /* nonstandard, update when there is a standard value */
    { AV_CODEC_ID_TSCC2       , 0xD0 }, /* nonstandard, camtasia uses it */
    { AV_CODEC_ID_EVRC        , 0xD1 }, /* nonstandard, pvAuthor uses it */
    { AV_CODEC_ID_VORBIS      , 0xDD }, /* nonstandard, gpac uses it */
    { AV_CODEC_ID_DVD_SUBTITLE, 0xE0 }, /* nonstandard, see unsupported-embedded-subs-2.mp4 */
    { AV_CODEC_ID_QCELP       , 0xE1 },
    { AV_CODEC_ID_MPEG4SYSTEMS, 0x01 },
    { AV_CODEC_ID_MPEG4SYSTEMS, 0x02 },
    { AV_CODEC_ID_NONE        ,    0 },
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

HRESULT MuxerFFmpeg::QueryInterface(const IID &iid, void **ppv)
{
    return m_pUnknowOuter->QueryInterface(iid, ppv);
}

ULONG MuxerFFmpeg::AddRef()
{
    return m_pUnknowOuter->AddRef();
}

ULONG MuxerFFmpeg::Release()
{
    return m_pUnknowOuter->Release();
}

HRESULT MuxerFFmpeg::NondelegatingQueryInterface(const IID &iid, void **ppv)
{
    if(iid == IID_IUNknown)
    {
        *ppv = static_cast<INondelegatingUnknown*>(this);
    }
    else if(iid == IID_IMUX)
    {
        *ppv = static_cast<IMux*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUNknown*>(*ppv)->AddRef();
    return S_OK;
}

ULONG MuxerFFmpeg::NondelegatingAddRef()
{
    return ++m_cRef;
}

ULONG MuxerFFmpeg::NondelegatingRelease()
{
    if(--m_cRef == 0)
    {
        std::cout << "COM Muxer FFmpeg delete." << std::endl;
        delete this;
        return 0;
    }
    return m_cRef;
}

MuxerFFmpeg::MuxerFFmpeg(IUNknown *pUnknowOuter)
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

MuxerFFmpeg::~MuxerFFmpeg()
{
    std::cout << "delete mxuer" << std::endl;
    clearDemuxer();
}

IMux::IMuxError MuxerFFmpeg::muxPacket(IUNknown *pUnknow)
{
    std::shared_ptr<AVPacket> packetPtr(av_packet_alloc(), [&](AVPacket* pPacket){
        av_packet_free(&pPacket);
    });

    getPacket(packetPtr.get(), pUnknow);

    if(av_write_frame(m_outputContext.get(), packetPtr.get()) < 0)
        return MuxError;

    return IMux::NoError;
}

std::vector<std::string> MuxerFFmpeg::getAllCodecString()
{
    std::vector<std::string> strCodecs;
    for (unsigned int i = 0; i < m_outputContext->nb_streams; i++) {
        strCodecs.push_back(getCodecString(m_outputContext->streams[i]->codecpar));
    }
    return strCodecs;
}

int MuxerFFmpeg::writePack(void *pUser, uint8_t *buf, int bufSize)
{
    IOCallBack* pCallback = (IOCallBack*)pUser;
    if(!(*pCallback))
        return bufSize;
    return (*pCallback)(buf, bufSize);
}

void MuxerFFmpeg::clearDemuxer()
{
    m_writeBuffer.reset();
    m_ioContext.reset();
    m_outputContext.reset();
}

IMux::IMuxError MuxerFFmpeg::addStream(IUNknown *pIStream)
{
    IPtr<IAVStream, &IID_ISTREAM> spIStream(pIStream);
    if(!spIStream)
        return ParamError;

    AVCodecID codecID = AV_CODEC_ID_NONE;
    auto codecIt = g_streamCodecTypeMap.find(spIStream->getStreamCodecType());
    if(codecIt != g_streamCodecTypeMap.end())
        codecID = codecIt->second;

    assert(codecID != AV_CODEC_ID_NONE);

    AVCodec *codec = avcodec_find_decoder(codecID);
    if(!codec)
    {
        return ParamError;
    }
    AVCodecContext* codecContext = avcodec_alloc_context3(codec);
    std::shared_ptr<AVCodecContext> codecContextPtr(codecContext, [&](AVCodecContext* codecContext){
       avcodec_free_context(&codecContext);
    });

    AVStream *outStream = avformat_new_stream(m_outputContext.get(), nullptr);
    if(!outStream)
        return CreateStreamError;

    avcodec_parameters_from_context(outStream->codecpar, codecContextPtr.get());
    codecParFFmpegMeida2FFmpeg(outStream->codecpar, pIStream);

//    int privateDataSize = 0;
//    AVStream *inStream = (AVStream*)spIStream->getPrivateData(privateDataSize);
//    AVStream *outStream = avformat_new_stream(m_outputContext.get(), nullptr);
//    avcodec_parameters_copy(outStream->codecpar, inStream->codecpar);

    return IMux::NoError;
}

IMux::IMuxError MuxerFFmpeg::initMuxer(IOCallBack outputCallback, std::string format)
{
    clearDemuxer();
    m_outputCallback = outputCallback;

    m_writeBuffer.reset((unsigned char*)av_malloc(BUF_SIZE), [&](unsigned char* data){
        if(m_ioContext && m_ioContext->buffer == data)
            av_free(data);
    });

    AVIOContext *pIOContext = avio_alloc_context(m_writeBuffer.get(), BUF_SIZE, 1, (void*)&(m_outputCallback),
                                                 nullptr, writePack, nullptr);
    m_ioContext.reset(pIOContext, [&](AVIOContext *ioContext) {
        av_free(ioContext);
    });

    AVFormatContext *pOutputContext = nullptr;
    if(avformat_alloc_output_context2(&pOutputContext, nullptr, format.c_str(), nullptr) < 0)
        return InitError;
    m_outputContext.reset(pOutputContext, [&](AVFormatContext *pOutputContext) {
        avformat_free_context(pOutputContext);
    });

    m_outputContext->pb = m_ioContext.get();
    m_outputContext->flags |= AVFMT_FLAG_CUSTOM_IO;
    m_outputContext->oformat->flags |= AVFMT_NOFILE;

    // rtp start time.
    m_outputContext->start_time = 0;
    m_outputContext->start_time_realtime = 0;
    if(format == "rtp")
        m_outputContext->packet_size = 1400;
    return IMux::NoError;
}

IMux::IMuxError MuxerFFmpeg::writeHeader()
{
    if(!(m_outputContext->oformat->flags & AVFMT_NOFILE))
        if(avio_open2(&m_outputContext->pb, m_outputContext->filename, AVIO_FLAG_WRITE, nullptr, nullptr) < 0)
            return OpenURLError;

    AVDictionary *formatOpts = nullptr;
    av_dict_set(&formatOpts, "rtsp_transport", "tcp", 0);
    av_dict_set(&formatOpts, "stimeout", "5000000", 0);
    av_dict_set(&formatOpts, "movflags", "empty_moov+default_base_moof+frag_keyframe", 0);

    IMux::IMuxError error = avformat_write_header(m_outputContext.get(), &formatOpts) < 0 ? WriteHeaderError: NoError;
    av_dict_free(&formatOpts);
    return error;
}

IMux::IMuxError MuxerFFmpeg::writeTrailer()
{
    av_write_trailer(m_outputContext.get());
    if(!(m_outputContext->oformat->flags & AVFMT_NOFILE)) {
        avio_close(m_outputContext->pb);
    }
    return IMux::NoError;
}

IMux::IMuxError MuxerFFmpeg::initMuxer(std::string url, std::string format)
{
    clearDemuxer();
    AVFormatContext *pOutputContext = nullptr;
    if(avformat_alloc_output_context2(&pOutputContext, nullptr, format.c_str(), url.c_str()) < 0)
        return InitError;
    m_outputContext.reset(pOutputContext, [&](AVFormatContext *pOutputContext){
        avformat_free_context(pOutputContext);
    });

    if(format == "rtp")
        m_outputContext->packet_size = 1400;
    return IMux::NoError;
}

void MuxerFFmpeg::codecParFFmpegMeida2FFmpeg(AVCodecParameters *par, IUNknown *pIStream)
{
    IPtr<IAVStream, &IID_ISTREAM> spStream(pIStream);
    if(!spStream || !par)
        return;

    if(spStream->getStreamType() == IAVStream::AVTypeVideo)
    {
        par->codec_type = AVMEDIA_TYPE_VIDEO;
        int width, height;
        spStream->getFrameSize(width, height);
        par->width = width;
        par->height = height;

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
    else if(spStream->getStreamType() == IAVStream::AVTypeAudio)
    {
        par->codec_type = AVMEDIA_TYPE_AUDIO;
        int audioChannels, sampleRate;
        IAVStream::AudioSampleFormat audioFormat;
        spStream->getAudioFormat(sampleRate, audioChannels, audioFormat);
        par->format = converAudioFormat(audioFormat);
        par->channels = audioChannels;
        par->sample_rate = sampleRate;

        par->extradata = nullptr;
        par->extradata_size = 0;
        if(par->codec_id == AV_CODEC_ID_AAC)
        {

            std::string inExtradata = spStream->getCodecExtradata();
            if(inExtradata.size() > 2)
            {
                parseAACHead(inExtradata.c_str(), audioChannels, sampleRate);
            }
            par->extradata = (uint8_t*)av_mallocz(2 + AV_INPUT_BUFFER_PADDING_SIZE);
            memset(par->extradata, 0, 2);
            par->extradata_size = 2;
            makeDsi(getSrIndex(sampleRate), audioChannels, par->extradata);
        }
    }
}

void MuxerFFmpeg::getPacket(AVPacket *pkt, IUNknown *pIPacket)
{
    if(pkt == nullptr)
        return;
    IPtr<IPacket, &IID_IPACKET> spIPacket(pIPacket);
    AVRational timebase = m_outputContext->streams[spIPacket->getStreamIndex()]->time_base;
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

int MuxerFFmpeg::converAudioFormat(IAVStream::AudioSampleFormat format)
{
    auto formatIt = g_streamAudioSampleFormatMap.find(format);
    if(formatIt != g_streamAudioSampleFormatMap.end())
        return formatIt->second;
    return AV_SAMPLE_FMT_NONE;
}

void MuxerFFmpeg::parseAACHead(const char pData[], int &channels, int &samplerate)
{
    int high = (pData[0] & 0x07) << 1;
    int low = (pData[1]&0x80);
    int sampleIndex = high + (low>>7);
    switch (sampleIndex) {
    case 0x0:
        samplerate = 96000;
        break;
    case 0x1:
        samplerate = 88200;
        break;
    case 0x2:
        samplerate = 48000;
        break;
    case 0x4:
        samplerate = 44100;
        break;
    case 0x5:
        samplerate = 32000;
        break;
    case 0x6:
        samplerate = 24000;
        break;
    case 0x7:
        samplerate = 22050;
        break;
    case 0x8:
        samplerate = 16000;
        break;
    case 0x9:
        samplerate = 12000;
        break;
    case 0xa:
        samplerate = 11025;
        break;
    case 0xb:
        samplerate = 8000;
        break;
    case 0xc:
        samplerate = 7350;
        break;
    default:
        samplerate = 44100;
        break;
    }
    channels = ((pData[1] - low) & 0xff) >> 3;
}

void MuxerFFmpeg::makeDsi(unsigned char samplingFrequencyIndex, unsigned char channels, unsigned char *dsi)
{
    unsigned char objectType = 0x02; // AAC LC by default
    dsi[0] = (objectType << 3) | (samplingFrequencyIndex >> 1);
    dsi[1] = ((samplingFrequencyIndex&1) << 7) | (channels << 3);
}

std::string MuxerFFmpeg::getCodecString(AVCodecParameters *par)
{
    const AVCodecTag *tags = nullptr;
    if(par->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        // tags = avformat_get_mov_video_tags();
    }
    else if (par->codec_type == AVMEDIA_TYPE_AUDIO)
    {
        // tags = avformat_get_mov_audio_tags();
    }
    else
    {
        return "";
    }

    int tag = av_codec_get_tag(&tags, par->codec_id);
    if (tag == 0)
        return "";

    char cType[5] = {0};
    AV_WL32(cType, tag);
    std::string strType(cType);
    if (strType == "mp4a") {
        std::string strCode = strType;
        auto codecIt = g_mp4ObjectType.find(par->codec_id);
        if(codecIt != g_mp4ObjectType.end())
        {
            char cOti[3] = {0};
            sprintf(cOti, "%02x", codecIt->second);
            strCode = strCode + "." + std::string(cOti);
        }
        if (par->extradata_size >= 2)
        {
            int aot = par->extradata[0] >> 3;
            if (aot == 31)
            {
                aot = ((AV_RB16(par->extradata) >> 5) & 0x3f) + 32;
            }
            char cAot[3] = {0};
            sprintf(cAot, "%x", aot);
            strCode = strCode + "." + cAot;
        }

        return strCode;
    }
    else if (strType == "avc1") {
        if (par->extradata_size > 4)
        {
            char cCodecString[7] = {0};
            sprintf(cCodecString, "%02x%02x%02x",
                    par->extradata[1], par->extradata[2], par->extradata[3]);
            return strType + "." + std::string(cCodecString);
        }
    }
    return "";
}

unsigned int MuxerFFmpeg::getSrIndex(unsigned int samples)
{
    unsigned int sampleIndex = 0;
    switch(samples)
    {
    case 96000:
        sampleIndex = 0x0;
        break;
    case 88200:
        sampleIndex = 0x1;
        break;
    case 64000:
        sampleIndex = 0x2;
        break;
    case 48000:
        sampleIndex = 0x3;
        break;
    case 44100:
        sampleIndex = 0x4;
        break;
    case 32000:
        sampleIndex = 0x5;
        break;
    case 24000:
        sampleIndex = 0x6;
        break;
    case 22050:
        sampleIndex = 0x7;
        break;
    case 16000:
        sampleIndex = 0x8;
        break;
    case 12000:
        sampleIndex = 0x9;
        break;
    case 11025:
        sampleIndex = 0xa;
        break;
    case 8000:
        sampleIndex = 0xb;
        break;
    case 7350:
        sampleIndex = 0xc;
        break;
    default:
        sampleIndex = 0x0;
        break;
    }
    return sampleIndex;
}

#if (!defined __APPLE__) && (!defined __ANDROED__) && (!defined DYNAMICLOADPLAYER)
IUNknown *CreateInstance(IUNknown *pUnknowOuter)
{
    std::cout <<"CreateInstance of MuxerFFmpeg" << std::endl;
    IUNknown* pI = (IUNknown*)(void*)new MuxerFFmpeg(pUnknowOuter);
    pI->AddRef();
    return pI;
}
#endif
