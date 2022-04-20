
//#include "aitraindecoder.h"
//#include "aitraindecoder.h"
#include <iostream>
#include "decoder.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cout << "no input file." << std::endl;
        return -1;
    }

    int num = 1;
    if(argc >= 3)
        num = std::stoi(std::string(argv[2]));

    for(int i = 0; i < num; i++)
    {
        std::cout << "codec index: " << i << std::endl;
        Decoder *pDecoder = new Decoder;
        pDecoder->setUrl(argv[1]);
        pDecoder->start();
    }

    do {
        CBaseThread::sleep(1000);
    } while(false);
    return 0;
}

