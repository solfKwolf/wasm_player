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

// {e01475a3-5b43-4d30-8be0-282aef56c422} Scale接口
extern "C" const IID IID_ISCALE =
{ 0xe01475a3, 0x5b43, 0x4d30, { 0x8b, 0xe0, 0x28, 0x2a, 0xef, 0x56, 0xc4, 0x22 } };

// {31ec2ad7-b834-41b9-b429-ff4241bc3045} Frame接口
extern "C" const IID IID_IFRAME =
{ 0x31ec2ad7, 0xb834, 0x41b9, { 0xb4, 0x29, 0xff, 0x42, 0x41, 0xbc, 0x30, 0x45 } };
