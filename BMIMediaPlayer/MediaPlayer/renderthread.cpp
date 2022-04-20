#include "renderthread.h"
#include "frame.h"
#include "masterclock.h"
#include "renderparamcontrol.h"
#include <iostream>

RenderThread::RenderThread(BaseMediaFactory *pMediaFac, int streamIndex)
    : m_pMediaFac(pMediaFac)
    , m_nStreamIndex(streamIndex)
    , m_packetBufferSize(0)
{

}

RenderThread::~RenderThread()
{

}

void RenderThread::getFrontFrameCallBack(GetFrontFrameCallBack getFrontFrame)
{
    m_getFrontFrame = getFrontFrame;
}

void RenderThread::stopRenderCallBack(StopRenderCallBack stopRender)
{
    m_stopRender = stopRender;
}

void RenderThread::getPacketCountCallBack(GetPacketCountCallBack getPacketCount)
{
    m_getPacketCount = getPacketCount;
}

void RenderThread::getPacketBufferSize(int nPacketBuffSize)
{
    m_packetBufferSize = nPacketBuffSize;
}

void RenderThread::sendFrameCallBack(SendFrameCallBack sendFrame)
{
    m_sendFrame = sendFrame;
}

void RenderThread::positionChangeCallBack(PositionChangeCallBack positionChange)
{
    m_positionChange = positionChange;
}

void RenderThread::setStreamCount(int nStreamCount)
{
    RenderParamControl::getInstance()->setStreamCount(nStreamCount);
}

void RenderThread::autoSpeed(bool autoSpeed)
{
    RenderParamControl::getInstance()->autoSpeed(autoSpeed);
}

void RenderThread::setSpeed(float fSpeed)
{
    MasterClock::getInstance()->setSpeed(fSpeed);
}

float RenderThread::getSpeed()
{
    return MasterClock::getInstance()->getSpeed();
}

int64_t RenderThread::getCodeRate()
{
    return RenderParamControl::getInstance()->getCodeRate();
}

void RenderThread::run()
{
    IUNknown *pFrameUNknown = nullptr;
    MasterClock::getInstance()->reset();
    int nFrameCount = 0;
    int nCaceCodeRate = 0;

    while(!isStop())
    {
        if(pFrameUNknown == nullptr)
        {
            pFrameUNknown = m_getFrontFrame(m_nStreamIndex);
        }
        if(pFrameUNknown == nullptr)
        {
            break;
        }
        IPtr<IFrame, &IID_IFRAME> spIFrame(pFrameUNknown);
        if(spIFrame->getFrameType() == IFrame::AVTypeVideo)
        {
            int packetCount = m_getPacketCount(m_nStreamIndex);
            RenderParamControl::getInstance()->autoSpeed(m_packetBufferSize, packetCount);
        }
        if(spIFrame->isEnd())
        {
            pFrameUNknown->Release();
            pFrameUNknown = nullptr;
            m_stopRender();
            break;
        }

        int nFramePts = spIFrame->getPts();
        int nFrameDur = spIFrame->getDuration();
        if(spIFrame->getFrameType() == IFrame::AVTypeVideo)
        {
            int nFrameRate = spIFrame->getFrameRate();
            int nSecond = RenderParamControl::getInstance()->getCodeRateSecond();
            if(nFrameRate * nSecond<= nFrameCount++)
            {
                RenderParamControl::getInstance()->setCodeRate(nCaceCodeRate/nSecond);
                nCaceCodeRate = 0;
                nFrameCount = 0;
            }
            nCaceCodeRate += spIFrame->getPacketSize();
        }
        long lClock = MasterClock::getInstance()->getClock();
        if(MasterClock::getInstance()->IsFirstFrame())
        {
            if(nFramePts>=0)
            {
                MasterClock::getInstance()->seekClock(nFramePts);
                lClock = MasterClock::getInstance()->getClock();
                MasterClock::getInstance()->setFirstFrame(false);
            }
        }

        if( abs(nFramePts-lClock)>2000 )
        {
            RenderParamControl::getInstance()->addSeekIndex();
            if(RenderParamControl::getInstance()->isSeekRender()||nFramePts<0)
            {
                RenderParamControl::getInstance()->resetSeekIndex();
                MasterClock::getInstance()->seekClock(nFramePts);
                lClock = MasterClock::getInstance()->getClock();
            }
            else
            {
                msleep(5);
                continue;
            }
        }

        if(nFramePts + nFrameDur <= lClock)
        {
            IPlay::ClockType clockType = RenderParamControl::getInstance()->getClockType();
            if(spIFrame->getFrameType() == IFrame::AVTypeVideo &&  clockType == IPlay::ClockType::VideoClock)
            {
                MasterClock::getInstance()->seekClock(nFramePts+nFrameDur);
                lClock = MasterClock::getInstance()->getClock();
            }
            else if(spIFrame->getFrameType() == IFrame::AVTypeAudio && clockType == IPlay::ClockType::AudioClock)
            {
                MasterClock::getInstance()->seekClock(nFramePts+nFrameDur);
                lClock = MasterClock::getInstance()->getClock();
            }
            else if(clockType == IPlay::OtherClock)
            {
                MasterClock::getInstance()->seekClock(nFramePts+nFrameDur);
            }
            else
            {
                if(pFrameUNknown)
                {
                    pFrameUNknown->Release();
                    pFrameUNknown = nullptr;
                }
            }
        }
        if (nFramePts > lClock)
        {
            msleep(nFramePts-lClock);
        }

        m_sendFrame(pFrameUNknown);

        if(pFrameUNknown)
        {
            if(spIFrame->getFrameType() == IFrame::AVTypeVideo)
            {
                m_positionChange(lClock/1000);
            }
            pFrameUNknown->Release();
            pFrameUNknown = nullptr;
        }
    }
}

