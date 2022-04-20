#include <iostream>
#include <packet.h>
#include <demux.h>
#include <fstream>
#include <thread>
#include "comdelegate.h"

int main(int argc, char *argv[])
{
#ifdef __WIN32__
    COMDelegate demuxerDelegate("ffmpegDemuxer.dll");
    COMDelegate packetDelegate("ffmpegPacket.dll");
#else
    COMDelegate demuxerDelegate("/home/boy/project/golang/src/streamplugins/BMIMediaPlayer/debug/libffmpegDemuxer.so");
    COMDelegate packetDelegate("/home/boy/project/golang/src/streamplugins/BMIMediaPlayer/debug/libffmpegPacket.so");
#endif

    IUNknown* pDemuxerUnknow = demuxerDelegate.createUnknow();
    if(!pDemuxerUnknow)
    {
        std::cout << "load ffmpeg demuxer error." << std::endl;
        return -1;
    }
    IUNknownDeleter demuxerDeleter(pDemuxerUnknow);

    IPtr<IDemux, &IID_IDEMUX> spIDemuxer(pDemuxerUnknow);

    if(spIDemuxer->initDemuxer("rtsp://192.168.1.103:10556/planback?channel=1&starttime=20181228134420&endtime=20181228151000&isreduce=0") != IDemux::NoError)
    {
        std::cout << "init demuxer error." << std::endl;
        return -1;
    }
    while(true)
    {
        static int i = 0;
        i++;
        if(i > 1000)
        {
            std::cout << "pause..." << std::endl;
            spIDemuxer->pause();
            std::this_thread::sleep_for(std::chrono::seconds(10));
            std::cout << "resume..." << std::endl;
            spIDemuxer->resume();
            i = 0;
        }
        IUNknown* pPacketUnknow = packetDelegate.createUnknow();
        IUNknownDeleter packetDeleter(pPacketUnknow);
        IDemux::IDemuxError error = spIDemuxer->demuxStream(pPacketUnknow);
        if(error == IDemux::NoError)
        {
            IUNknown *pConverPacket = packetDelegate.createUnknow();
            IUNknownDeleter converPacketDeleter(pConverPacket);
            IPtr<IPacket, &IID_IPACKET> spIPacket(pConverPacket);
            IDemux::IDemuxError error = spIDemuxer->converToNAL(pConverPacket, pPacketUnknow);
            if(error == IDemux::DemuxError || error == IDemux::NeedlessConver)
                spIPacket = pPacketUnknow;
            if(spIPacket)
            {
                std::fstream file;
                file.open(std::string("D:\\test\\") + std::to_string(spIPacket->getStreamIndex()), std::ios_base::out | std::ios_base::app | std::ios_base::binary);
                int nPacketSize = 0;
                const unsigned char* pData = spIPacket->getPacketData(nPacketSize);
                file.write((const char*)pData, nPacketSize);
                file.close();
                std::cout << "write data size: " << nPacketSize << std::endl;
            }
        }
        else if(error == IDemux::DemuxerEof)
        {
            std::cout << "demux eof." << std::endl;
            break;
        }
        else
        {
            std::cout << "demux error." << std::endl;
            break;
        }
    }
}
