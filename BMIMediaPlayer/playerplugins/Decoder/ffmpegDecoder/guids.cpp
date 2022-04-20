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

// {b8239d7a-8fa1-491f-ba27-86d4d0fa108e} 解码器接口
extern "C" const IID IID_IDECODE =
{ 0xb8239d7a, 0x8fa1, 0x491f, { 0xba, 0x27, 0x86, 0xd4, 0xd0, 0xfa, 0x10, 0x8e } };

// {b041c5c0-7fd1-4f4a-a909-5271da245e21} Packet接口
extern "C" const IID IID_IPACKET =
{ 0xb041c5c0, 0x7fd1, 0x4f4a, { 0xa9, 0x09, 0x52, 0x71, 0xda, 0x24, 0x5e, 0x21 } };

// {69fa0de6-ad5a-4716-8254-d44d823c9e09} Stream接口
extern "C" const IID IID_ISTREAM =
{ 0x69fa0de6, 0xad5a, 0x4716, { 0x82, 0x54, 0xd4, 0x4d, 0x82, 0x3c, 0x9e, 0x09 } };

// {31ec2ad7-b834-41b9-b429-ff4241bc3045} Frame接口
extern "C" const IID IID_IFRAME =
{ 0x31ec2ad7, 0xb834, 0x41b9, { 0xb4, 0x29, 0xff, 0x42, 0x41, 0xbc, 0x30, 0x45 } };
