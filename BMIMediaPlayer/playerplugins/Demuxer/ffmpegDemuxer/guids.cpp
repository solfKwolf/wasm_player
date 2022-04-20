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

// {a3a012e1-5a3b-4e95-8374-8ede2af91b58} 解复用接口
extern "C" const IID IID_IDEMUX =
{ 0xa3a012e1, 0x5a3b, 0x4e95, { 0x83, 0x74, 0x8e, 0xde, 0x2a, 0xf9, 0x1b, 0x58 } };

// {b041c5c0-7fd1-4f4a-a909-5271da245e21} Packet接口
extern "C" const IID IID_IPACKET =
{ 0xb041c5c0, 0x7fd1, 0x4f4a, { 0xa9, 0x09, 0x52, 0x71, 0xda, 0x24, 0x5e, 0x21 } };

// {69fa0de6-ad5a-4716-8254-d44d823c9e09} Stream接口
extern "C" const IID IID_ISTREAM =
{ 0x69fa0de6, 0xad5a, 0x4716, { 0x82, 0x54, 0xd4, 0x4d, 0x82, 0x3c, 0x9e, 0x09 } };
