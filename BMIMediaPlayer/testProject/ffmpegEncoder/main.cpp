
//#include "aitraindecoder.h"
//#include "aitraindecoder.h"
#include <iostream>
#include "encoder.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cout << "no input file." << std::endl;
        return -1;
    }

    Decoder *pDecoder = new Decoder;
    pDecoder->setUrl(argv[1], "rtsp://192.168.1.247:10554/test.sdp", "rtsp");
    pDecoder->start();

    while(true)
    {
        pDecoder->start();

        CBaseThread::sleep(10);

        pDecoder->stop();
        CBaseThread::sleep(1);
    }
}

