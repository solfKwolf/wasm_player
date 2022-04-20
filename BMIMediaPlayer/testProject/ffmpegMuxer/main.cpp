#include <iostream>
#include <packet.h>
#include <demux.h>
#include <fstream>
#include <mux.h>
#include <stream.h>
#include "comdelegate.h"
#include <fstream>
#include <thread>

int writeFile(unsigned char* data, int len) {
    std::fstream file;
    file.open(std::string("D:\\test\\test-2.mp4"), std::ios_base::out | std::ios_base::app | std::ios_base::binary);
    file.write((const char*)data, len);
    file.close();
    std::cout << "out data: " << len << std::endl;
    return len;
}

class FileWriter : public IOWriter {
public:
    virtual int64_t Write(unsigned char *data, unsigned long len) {
        std::fstream file;
        file.open(std::string("D:\\test\\test-2.mp4"), std::ios_base::out | std::ios_base::app | std::ios_base::binary);
        file.write((const char*)data, len);
        file.close();
        std::cout << "out data: " << len << std::endl;
        return len;
    }
};

class FileReader : public IOReader {
public:
    FileReader() {
        file.open("D:\\testfiles\\xiamu.flv", std::ios_base::in | std::ios_base::binary);
    }

    ~FileReader() {
        file.close();
    }

    virtual int64_t Read(unsigned char* data, unsigned long len) {
        file.read((char*)data, len);
        return file.gcount();
    }

    virtual int64_t Seek(int64_t pos, unsigned long whence) {
        file.seekg(pos + whence);
        return file.gcount();
    }

private:
    std::fstream file;

};

int main(int argc, char *argv[])
{
    COMDelegate demuxerDelegate("ffmpegDemuxer.dll");
    COMDelegate packetDelegate("ffmpegPacket.dll");
    COMDelegate muxerDelegate("ffmpegMuxer.dll");
    COMDelegate streamDelegate("ffmpegStream.dll");

    IUNknown* pDemuxerUnknow = demuxerDelegate.createUnknow();
    if(!pDemuxerUnknow)
    {
        std::cout << "load ffmpeg demuxer error." << std::endl;
        return -1;
    }
    IUNknownDeleter demuxerDeleter(pDemuxerUnknow);

    IOReader *reader = new FileReader;

    IPtr<IDemux, &IID_IDEMUX> spIDemuxer(pDemuxerUnknow);
    if(spIDemuxer->initDemuxer(reader) != IDemux::NoError)
    {
        std::cout << "init demuxer error." << std::endl;
        return -1;
    }

    IUNknown* pMuxerUnknow = muxerDelegate.createUnknow();
    if(!pMuxerUnknow)
    {
        std::cout << "load ffmpeg muxer error." << std::endl;
        return -1;
    }

//    IUNknownDeleter muxerDeleter(pMuxerUnknow);

    IOWriter *writer = new FileWriter;
    IPtr<IMux, &IID_IMUX> spIMuxer(pMuxerUnknow);
    if(spIMuxer->initMuxer(writer, "mp4") != IMux::NoError)
    {
        std::cout << "init muxer error." << std::endl;
        return -1;
    }

    for(int i = 0; i < spIDemuxer->getStreamCount(); i++)
    {
        IUNknown* pInStream = streamDelegate.createUnknow();
        IUNknown* pOutStream = streamDelegate.createUnknow();
        IPtr<IAVStream, &IID_ISTREAM> spInStream(pInStream);
        IPtr<IAVStream, &IID_ISTREAM> spOutStream(pOutStream);
        spIDemuxer->getStream(i, pInStream);

        spOutStream->setStreamCodecType(spInStream->getStreamCodecType());
        spOutStream->setStreamType(spInStream->getStreamType());

        // video
        if(spOutStream->getStreamType() == IAVStream::AVTypeVideo)
        {
            int width, height;
            spInStream->getFrameSize(width, height);
            spOutStream->setFrameSize(width, height);
            spOutStream->setCodecExtradata(spInStream->getCodecExtradata());
        }
        else if(spOutStream->getStreamType() == IAVStream::AVTypeAudio)
        {
            // audio
            int audioChannels, sampleRate;
            IAVStream::AudioSampleFormat audioFormat;
            spInStream->getAudioFormat(sampleRate, audioChannels, audioFormat);
            spOutStream->setAudioFormat(sampleRate, audioChannels, audioFormat);
            spOutStream->setCodecExtradata(spInStream->getCodecExtradata());

            int frameSize;
            spInStream->getFrameSize(frameSize, frameSize);
            spOutStream->setFrameSize(frameSize, frameSize);
        }

        if(spIMuxer->addStream(pOutStream) != IMux::NoError)
        {
            std::cout << "add stream error." << std::endl;
        }

        pInStream->Release();
        pOutStream->Release();
    }

    auto codecStrings = spIMuxer->getAllCodecString();

    if(spIMuxer->writeHeader() != IMux::NoError)
    {
        std::cout << "write header error." << std::endl;
        return -1;
    }

    while(true)
    {
        IUNknown* pPacketUnknow = packetDelegate.createUnknow();
        IUNknownDeleter packetDeleter(pPacketUnknow);
        IDemux::IDemuxError error = spIDemuxer->demuxStream(pPacketUnknow);
        if(error == IDemux::NoError)
        {
            IUNknown *pConverPacket = packetDelegate.createUnknow();
            IUNknownDeleter converPacketDeleter(pConverPacket);
            IPtr<IPacket, &IID_IPACKET> spIPacket(pPacketUnknow);
            if(spIPacket)
            {
//                if(spIPacket->getStreamIndex() == 1)
//                    continue;
                if(spIMuxer->muxPacket(spIPacket.get()) != IMux::NoError)
                    std::cout << "mux packet error." << std::endl;
            }
        }
        else if(error == IDemux::DemuxerEof)
        {
            std::cout << "demux eof." << std::endl;
            spIMuxer->writeTrailer();
            break;
        }
        else
        {
            std::cout << "demux error." << std::endl;
            break;
        }
    }
    return 0;
}
