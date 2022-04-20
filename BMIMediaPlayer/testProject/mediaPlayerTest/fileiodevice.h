#ifndef FILEIODEVICE_H
#define FILEIODEVICE_H

#include <QFile>

class FileIODevice : public QFile
{
public:
    FileIODevice(QString strFileName);
    FileIODevice();
    ~FileIODevice();

    int64_t readData(unsigned char* buffer, int len);
    int64_t seekData(int64_t offset, int whence);

};

#endif // FILEIODEVICE_H
