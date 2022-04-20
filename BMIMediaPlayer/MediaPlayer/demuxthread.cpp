#include <demux.h>
#include "demuxthread.h"
#include <iostream>


DemuxThread::DemuxThread(BaseMediaFactory *mediaFac,
                         IUNknown *pDemuxerUnknow,
                         OnRecvPacketCallBack recvPacket)
    : m_pDemuxerUnknow(pDemuxerUnknow)
    , m_pMediaFac(mediaFac)
    , m_recvPacket(recvPacket)
{
    std::cout << "create a DemuxThread."<<std::endl;
    m_seekInfo.init();
    m_scaleInfo.init();
    m_pauseInfo.init();
}

DemuxThread::~DemuxThread()
{

}

void DemuxThread::run()
{
    IPtr<IDemux,&IID_IDEMUX> spIDemux(m_pDemuxerUnknow);
    if(!spIDemux)
    {
        std::cout<<("DemuxThread::run: Cannot create a smart pointer to IDmux.");
        return;
    }

    while(!isStop())
    {
        controlDemux();
        IUNknown* pPacketUnknow = m_pMediaFac->createPacketUNknown();
        m_mutex.lock();
        IDemux::IDemuxError error = spIDemux->demuxStream(pPacketUnknow);
        m_mutex.unlock();
        if (error == IDemux::NoError)
        {
            m_recvPacket(pPacketUnknow);
        }
        else if(error == IDemux::DemuxerEof)
        {
            std::cout << "DemuxThread::run: demux eof." << std::endl;
            m_recvPacket(pPacketUnknow);
            stop();
        }
        else
        {
            std::cout << "DemuxThread::run: demux error." << std::endl;
//            break;
        }
        pPacketUnknow->Release();
    }
}

void DemuxThread::controlDemux()
{
    IPtr<IDemux, &IID_IDEMUX> spIDemux(m_pDemuxerUnknow);
    if (!spIDemux)
    {
        std::cout<<"Cannot create a smart pointer to IDmux."<<std::endl;
        return ;
    }
    if(m_pauseInfo.m_isPause)
    {
        m_pauseInfo.waitPause();
    }

    if(m_seekInfo.m_needSeek)
    {
        if (spIDemux->seek(m_seekInfo.m_nSeekSecond))
        {
            m_clearPacket();
            m_seekInfo.m_needSeek = false;
            m_seekInfo.m_nSeekSecond = 0;
        }
    }

    if(m_scaleInfo.m_needScale)
    {
        spIDemux->setScale(m_scaleInfo.m_fScale);
        m_scaleInfo.m_needScale = false;
        m_scaleInfo.m_fScale = 1.0;
    }
}

void DemuxThread::pause()
{
    if (m_pDemuxerUnknow)
    {
        IPtr<IDemux, &IID_IDEMUX> spIDemux(m_pDemuxerUnknow);
        if (!spIDemux)
        {
            std::cout<<"Cannot create a smart pointer to IDmux."<<std::endl;
            return ;
        }
        std::cout << "pause..." << std::endl;
        std::unique_lock<std::mutex> locker(m_seekMutex);
        m_pauseInfo.m_isPause = true;
        spIDemux->pause();
    }
}

void DemuxThread::resume()
{
    if (m_pDemuxerUnknow)
    {
        IPtr<IDemux, &IID_IDEMUX> spIDemux(m_pDemuxerUnknow);
        if (!spIDemux)
        {
            std::cout<<"Cannot create a smart pointer to IDmux."<<std::endl;
            return ;
        }
        std::cout << "Resume..." << std::endl;
        m_pauseInfo.checkPause();
        std::unique_lock<std::mutex> locker(m_seekMutex);
        spIDemux->resume();
    }
}

bool DemuxThread::seek(int nSecond)
{
    if(m_pDemuxerUnknow)
    {
        m_seekInfo.m_needSeek = true;
        m_seekInfo.m_nSeekSecond = nSecond;
        return true;
    }
    return false;
}

void DemuxThread::setScale(float scale)
{
    if(m_pDemuxerUnknow)
    {
        m_scaleInfo.m_needScale = true;
        m_scaleInfo.m_fScale = scale;
    }
}

void DemuxThread::forceWakePause()
{
    m_pauseInfo.forcewakePause();
}

void DemuxThread::clearPacket(ClearPacketCallBack clearPacket)
{
    m_clearPacket = clearPacket;
}

