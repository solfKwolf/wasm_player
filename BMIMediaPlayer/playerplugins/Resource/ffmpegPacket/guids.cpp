#ifdef __WIN32__
#include <Windows.h>
#include <windef.h>
#else
#include "linuxdef.h"
#include <stdint.h>
#endif // __WIN32__

extern "C" const IID IID_IUNknown = {
    0,0,0,{0,0,0,0,0,0,0,0}
};

// {b041c5c0-7fd1-4f4a-a909-5271da245e21} Packet接口
extern "C" const IID IID_IPACKET =
{ 0xb041c5c0, 0x7fd1, 0x4f4a, { 0xa9, 0x09, 0x52, 0x71, 0xda, 0x24, 0x5e, 0x21 } };
