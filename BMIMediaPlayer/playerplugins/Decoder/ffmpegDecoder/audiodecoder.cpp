extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}
#include "audiodecoder.h"
#include <packet.h>
#include <frame.h>
#include <stream.h>

AudioDecoder::AudioDecoder(AVCodecContext *pCodecContext, IUNknown *pIStream)
    : RealDecoder(pCodecContext, pIStream)
    , m_pFrame(nullptr)
{
    m_pFrame.reset(av_frame_alloc(), [&](AVFrame *pFrame){
        av_frame_free(&pFrame);
    });
}

AudioDecoder::~AudioDecoder()
{
    m_pFrame.reset();
}

IDecode::IDecodeError AudioDecoder::decodeData(IUNknown *pIPacket, IUNknown *pIFrame)
{
    IPtr<IPacket, &IID_IPACKET> spIPacket(pIPacket);
    IPtr<IFrame, &IID_IFRAME> spIFrame(pIFrame);
    IPtr<IAVStream, &IID_ISTREAM> spIStream(m_pIStream.get());
    if(!spIPacket || !spIFrame)
        return IDecode::ParamError;
    if(spIPacket->packetName() != "FFMPEG PACKET")
        return IDecode::ParamError;

    AVPacket* pPacket = av_packet_alloc();
    if(!spIPacket->isEnd())
        getPacket(pPacket, pIPacket, m_pIStream.get());

    int ret = -1;

    ret = avcodec_send_packet(m_pCodecContext.get(), pPacket);
    av_packet_free(&pPacket);
    if(ret == AVERROR(EINVAL) || ret == AVERROR(ENOMEM))
        return IDecode::DecodeError;

    ret = avcodec_receive_frame(m_pCodecContext.get(), m_pFrame.get());
    if(spIPacket->isEnd() && ret == AVERROR_EOF)
    {
        spIFrame->setEofFlag(true);
        spIFrame->setStreamIndex(spIStream->getStreamIndex());
        return IDecode::DecodeEof;
    }
    else if(ret == AVERROR(EAGAIN))
        return IDecode::FrameUseless;
    else if(ret < 0)
        return IDecode::DecodeError;

    if(m_pFrame->pts == AV_NOPTS_VALUE)
        m_pFrame->pts = av_frame_get_best_effort_timestamp(m_pFrame.get());

    AVFrame* frame = av_frame_alloc();
    av_frame_ref(frame, m_pFrame.get());
    if(frame == nullptr)
    {
        return IDecode::FrameUseless;
    }

//    auto releaseFunc = [&](void* pFrameData, int nPacketLen)
//    {
//        AVFrame* frame = (AVFrame*)pFrameData;
//        av_frame_free(&frame);
//    };
//    spIFrame->setPrivateData(frame, sizeof(AVFrame));
//    spIFrame->setReleaseFunction(releaseFunc);

    IAVStream::StreamRational timebase = spIStream->getTimebase();
    AVRational ffmpegTimebase;
    ffmpegTimebase.num = timebase.num;
    ffmpegTimebase.den = timebase.den;

    spIFrame->setPts(frame->pts * 1000 * av_q2d(ffmpegTimebase));
    spIFrame->setStreamIndex(spIStream->getStreamIndex());
    spIFrame->setFrameType(IFrame::AVTypeAudio);

    int channels = 0;
    int sampleRate = 0;
    IAVStream::AudioSampleFormat sampleFmt = IAVStream::AudioFormateNone;
    spIStream->getAudioFormat(sampleRate, channels, sampleFmt);
    spIFrame->setAudioChannels(channels);
    spIFrame->setSampleRate(sampleRate);
    spIFrame->setAudioSampleFormat(IFrame::AudioSampleFormat(sampleFmt));
    spIFrame->setNBSamples(frame->nb_samples);

    IAVStream::StreamRational framerate = spIStream->getFrameRate();
    if(frame->pkt_duration <= 0 && framerate.num != 0 && framerate.den != 0)
    {
        int64_t nCalcDuration = (double)AV_TIME_BASE / (framerate.num/framerate.den);
        int64_t duration = nCalcDuration / (double)(av_q2d(ffmpegTimebase)*AV_TIME_BASE);
        spIFrame->setDuration(duration);
    }
    else
    {
        int64_t duration = frame->pkt_duration * 1000 * av_q2d(ffmpegTimebase);
        spIFrame->setDuration(duration);
    }

    int frameDataSize = frame->linesize[0]*frame->channels;
    unsigned char* frameData = (unsigned char*)av_malloc(frameDataSize);
    for(int i = 0; i < frame->channels; i++)
        memcpy(frameData + i*frame->linesize[0], frame->data[i], frame->linesize[0]);
    spIFrame->setFrameData(frameData, frameDataSize);
    auto releaseFunc = [&](void *frameData)
    {
        av_free(frameData);
    };
    spIFrame->setPrivateData(frameData, releaseFunc);

    av_frame_free(&frame);
    return IDecode::NoError;
}

