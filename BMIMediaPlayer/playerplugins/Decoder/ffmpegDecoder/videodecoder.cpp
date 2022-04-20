extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}
#include "videodecoder.h"
#include <packet.h>
#include <frame.h>
#include <stream.h>
#include <iostream>
#include <map>

static std::map<IFrame::FrameFormat, AVPixelFormat> g_frameFrameFormatMap =
{
    {IFrame::FormatNone, AV_PIX_FMT_NONE},
    {IFrame::FormatRGB24, AV_PIX_FMT_RGB24},
    {IFrame::FormatRGBA, AV_PIX_FMT_RGBA},
    {IFrame::FormatBGRA, AV_PIX_FMT_BGRA},
    {IFrame::FormatGRAY8, AV_PIX_FMT_GRAY8},
    {IFrame::FormatYUV420P, AV_PIX_FMT_YUV420P},
    {IFrame::ForamtARGB, AV_PIX_FMT_ARGB},
    {IFrame::ForamtNV12, AV_PIX_FMT_NV12},
    {IFrame::ForamtNV21, AV_PIX_FMT_NV21},
};


VideoDecoder::VideoDecoder(AVCodecContext *pCodecContext, IUNknown *pIStream)
    : RealDecoder(pCodecContext, pIStream)
{
    m_pFrame.reset(av_frame_alloc(), [&](AVFrame *pFrame) {
        av_frame_free(&pFrame);
    });
}

VideoDecoder::~VideoDecoder()
{
    m_imgConvert.reset();
    m_pFrame.reset();
}

IDecode::IDecodeError VideoDecoder::decodeData(IUNknown *pIPacket, IUNknown *pIFrame)
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
    if(m_pFrame->data[0] != nullptr && m_pFrame->data[0][0] == 0)
    {
        std::cout << "frame data is error..." << std::endl;
    }

    if(m_pFrame->pts == AV_NOPTS_VALUE)
        m_pFrame->pts = av_frame_get_best_effort_timestamp(m_pFrame.get());

    AVFrame* frame = av_frame_alloc();
    av_frame_ref(frame, m_pFrame.get());
    if(frame == nullptr)
    {
        return IDecode::FrameUseless;
    }

    int nPacketSize = 0;

    spIPacket->getPacketData(nPacketSize);
    spIFrame->setPacketSize(nPacketSize);
    spIFrame->setStreamIndex(spIStream->getStreamIndex());
    spIFrame->setFrameSize(frame->width, frame->height);

    IAVStream::StreamRational timebase = spIStream->getTimebase();
    AVRational ffmpegTimebase;
    ffmpegTimebase.num = timebase.num;
    ffmpegTimebase.den = timebase.den;

    spIFrame->setPts(frame->pts * 1000 * av_q2d(ffmpegTimebase));

    IAVStream::StreamRational framerate = spIStream->getFrameRate();
    spIFrame->setFrameRate(framerate.num/framerate.den);
    spIFrame->setFrameType(IFrame::AVTypeVideo);
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

    std::function<void(AVFrame*)> releaseFrame;
    AVFrame *enableFrame = nullptr;
    if(frame->format == AV_PIX_FMT_RGB24 || frame->format == AV_PIX_FMT_BGRA || frame->format == AV_PIX_FMT_YUV420P || frame->format == AV_PIX_FMT_RGBA)
    {
        releaseFrame = [&](AVFrame *frame)
        {
            av_frame_free(&frame);
        };
        IFrame::FrameFormat format = IFrame::FormatNone;

        for(auto it = g_frameFrameFormatMap.begin(); it != g_frameFrameFormatMap.end(); it++)
        {
            if(it->second == frame->format)
            {
                format = it->first;
                break;
            }
        }
        spIFrame->setFrameFormat(format);
        enableFrame = frame;
    }
    else
    {
        if(!m_imgConvert)
            m_imgConvert.reset(initConverContext(frame->format, frame->width, frame->height, IFrame::FormatYUV420P, frame->width, frame->height),
                               sws_freeContext);

        int nOutsize = getPictureSize(IFrame::FormatYUV420P, frame->width, frame->height);
        unsigned char* pOutdata = (unsigned char*)av_malloc(nOutsize);

        AVFrame *pFrameScale = av_frame_alloc();
        av_image_fill_arrays(pFrameScale->data, pFrameScale->linesize, (uint8_t*)pOutdata, (enum AVPixelFormat)convertFormat(IFrame::FormatYUV420P), frame->width, frame->height, 1);

        converImg(frame->data, frame->linesize, frame->height, pFrameScale->data, pFrameScale->linesize);

        releaseFrame = [&](AVFrame *frame)
        {
            if(frame->data[0])
                av_freep(&frame->data[0]);
            av_frame_free(&frame);
        };
        pFrameScale->pts = frame->pts;
        pFrameScale->format = AV_PIX_FMT_YUV420P;
        pFrameScale->width = frame->width;
        pFrameScale->height = frame->height;
        spIFrame->setFrameFormat(IFrame::FormatYUV420P);
        enableFrame= pFrameScale;
        av_frame_free(&frame);
    }

    int frameDataSize = av_image_get_buffer_size((AVPixelFormat)enableFrame->format, enableFrame->width, enableFrame->height, 1);
    unsigned char* frameData = (unsigned char*)av_malloc(frameDataSize);
    av_image_copy_to_buffer(frameData, frameDataSize, enableFrame->data, enableFrame->linesize, (AVPixelFormat)enableFrame->format, enableFrame->width, enableFrame->height, 1);
    releaseFrame(enableFrame);
    spIFrame->setFrameData(frameData, frameDataSize);
    auto releaseFunc = [&](void *frameData)
    {
        av_free(frameData);
    };
    spIFrame->setPrivateData(frameData, releaseFunc);

    return IDecode::NoError;
}

SwsContext *VideoDecoder::initConverContext(int inPix, int inWidth, int inHeight, IFrame::FrameFormat outFormat, int outWidth, int outHeight)
{
    enum AVPixelFormat outPix = (enum AVPixelFormat)convertFormat(outFormat);
    SwsContext* imgConvert = nullptr;
    imgConvert = sws_getCachedContext(imgConvert, inWidth, inHeight, (enum AVPixelFormat)inPix, outWidth, outHeight, outPix, SWS_BICUBIC, nullptr, nullptr, nullptr);
    return imgConvert;
}

int VideoDecoder::converImg(unsigned char* inData[], int inLen[], int inHeight, unsigned char* outData[], int outLen[])
{
    if(outData == nullptr)
        return 0;
    if(!m_imgConvert)
        return 0;
    return sws_scale(m_imgConvert.get(), inData, inLen, 0, inHeight, outData, outLen);
}

int VideoDecoder::convertFormat(IFrame::FrameFormat format)
{
    auto it = g_frameFrameFormatMap.find(format);
    if (it != g_frameFrameFormatMap.end())
        return it->second;
    return AV_PIX_FMT_NONE;
}

int VideoDecoder::getPictureSize(IFrame::FrameFormat format, int nWidth, int nHeight)
{
    return av_image_get_buffer_size((enum AVPixelFormat)convertFormat(format) , nWidth, nHeight, 1);
}

