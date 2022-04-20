#ifndef VIDEODECODER_H
#define VIDEODECODER_H
#include "realdecoder.h"
#include <frame.h>
#include <memory>

extern "C"
{
    struct SwsContext;
    struct AVFrame;
    struct AVPacket;
}

typedef std::shared_ptr<SwsContext> SwsContextPtr;
typedef std::shared_ptr<AVFrame> AVFramePtr;

class VideoDecoder : public RealDecoder
{
public:
    VideoDecoder(AVCodecContext *pCodecContext, IUNknown* pIStream);
    ~VideoDecoder();
public:
    virtual IDecode::IDecodeError decodeData(IUNknown* pIPacket, IUNknown* pIFrame) override;

private:
    int converImg(unsigned char* inData[], int inLen[], int inHeight, unsigned char* outData[], int outLen[]);
    SwsContext* initConverContext(int inPix, int inWidth, int inHeight,
                                  IFrame::FrameFormat outFormat, int outWidth, int outHeight);
    int convertFormat(IFrame::FrameFormat format);
    int getPictureSize(IFrame::FrameFormat format, int nWidth, int nHeight);

    SwsContextPtr m_imgConvert;
    AVFramePtr m_pFrame;
};

#endif // VIDEODECODER_H
