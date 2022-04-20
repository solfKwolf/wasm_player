#include <decode.h>
#include "decodethread.h"
#include <iostream>
#include "packet.h"

DecodeThread::DecodeThread(BaseMediaFactory *mediaFac,
                           IUNknown *pDecoderUnknow,
                           OnGetPacketCallBack getPacket,
                           OnSendFrameCallBack sendFrame,
                           int stream_index)
    : m_pDecoderUnknow(pDecoderUnknow)
    , m_pMediaFac(mediaFac)
    , m_getPacket(getPacket)
    , m_sendFrame(sendFrame)
    , n_streamIndex(stream_index)
{
    std::cout<<"create a DecodeThread."<<std::endl;
}

DecodeThread::~DecodeThread()
{

}

void DecodeThread::run()
{
    IPtr<IDecode, &IID_IDECODE> spIDecode(m_pDecoderUnknow);
    if (!spIDecode)
    {
        std::cout<<("DecodeThread::run: Cannot create a smart pointer to IDecode.")<<std::endl;
        return;
    }
    while(!isStop())
    {
        IUNknown* pPacketUnknow = m_getPacket(n_streamIndex);
        if(pPacketUnknow == nullptr)
        {
            continue;
        }
        IUNknown* pFrameUnknow = m_pMediaFac->createFrameUNknown();

        IPtr<IPacket, &IID_IPACKET> spIPacket(pPacketUnknow);
        IDecode::IDecodeError errorCode = IDecode::NoError;

        if (spIPacket->isEnd())
        {
            while(errorCode != IDecode::DecodeEof)
            {
                errorCode = spIDecode->decodeData(pPacketUnknow,pFrameUnknow);
                m_sendFrame(pFrameUnknow);
                pFrameUnknow->Release();
                pFrameUnknow = m_pMediaFac->createFrameUNknown();
            }
        }
        else
            errorCode = spIDecode->decodeData(pPacketUnknow,pFrameUnknow);

        if (errorCode == IDecode::NoError)
        {
            m_sendFrame(pFrameUnknow);
        }
        else if(errorCode == IDecode::DecodeEof)
        {
            std::cout << "DecodeThread::run: decode eof."<<std::endl;
            stop();
        }
        else if(errorCode ==IDecode::FrameUseless)
        {
            std::cout << "DecodeThread::run: no frame." << std::endl;
        }
        else
        {
            std::cout << "DecodeThread::run: decode error."<<errorCode<<std::endl;
        }
        pFrameUnknow->Release();
        if(pPacketUnknow)
        {
            pPacketUnknow->Release();
        }
    }
}







