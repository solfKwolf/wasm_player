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

// {69fa0de6-ad5a-4716-8254-d44d823c9e09} Stream接口
extern "C" const IID IID_ISTREAM =
{ 0x69fa0de6, 0xad5a, 0x4716, { 0x82, 0x54, 0xd4, 0x4d, 0x82, 0x3c, 0x9e, 0x09 } };
