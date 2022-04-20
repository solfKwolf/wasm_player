#include "fileiodevice.h"

FileIODevice::FileIODevice(QString strFileName)
    : QFile(strFileName)
{

}

FileIODevice::FileIODevice()
{

}

FileIODevice::~FileIODevice()
{

}

int64_t FileIODevice::readData(unsigned char *buffer, int len)
{
    return QFile::read((char*)buffer, len);
}

int64_t FileIODevice::seekData(int64_t offset, int whence)
{
    QFile::seek(offset+whence);
    return offset+whence;
}

