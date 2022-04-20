#ifndef AITRAINDECODER_H
#define AITRAINDECODER_H

#include "comdelegate.h"

class AITrainDecoder
{
public:
    AITrainDecoder();
    ~AITrainDecoder();

    bool DecodeFile(std::string &url,int framenum, std::string &dstpath);

private:
    int decode(IUNknown* pPacketUnknow, IUNknown* pDecoderUnknow, IUNknown* pFrameUnknow, IUNknown *pScaleUnknow);
    COMDelegate m_demuxerDelegate;
    COMDelegate m_decoderDelegate;
    COMDelegate m_packetDelegate;
    COMDelegate m_streamDelegate;
    COMDelegate m_frameDelegate;
    COMDelegate m_scaleDelegate;
    std::string m_dstpath;
    int m_nFrameNum;
};

#endif // AITRAINDECODER_H
