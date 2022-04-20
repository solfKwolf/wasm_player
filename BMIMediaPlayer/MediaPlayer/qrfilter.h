#ifndef QRFILTER_H
#define QRFILTER_H
#include <string>

class QRFilter
{
    enum Version {
        QRcodeVersion = 2
    };

public:
    struct ImgRect
    {
        int x;
        int y;
        int width;
        int height;
    };

    enum ImgFormat
    {
        IMG_YUV420
    };

    struct QRRectImg
    {
        int width;
        int height;
        unsigned char *buffer;
    };

    struct imageYUV
    {
        int qrcode_x;
        int qrcode_y;
        int width;
        int height;
        unsigned char *buffer;
    };

public:
    QRFilter();
    virtual ~QRFilter();

    virtual void addQRCode(unsigned char* pImg, ImgFormat imgFmt, ImgRect rect, std::string strText, ImgRect qrcodeRect);
    virtual void clearQRCode();

private:
    void blend(imageYUV* frame, QRRectImg *img);
    template<typename T>
    T getMin(T value1, T value2)
    {
        return value1 < value2 ? value1 : value2;
    }

    QRRectImg *GenerateQRcode(std::string strText, int width);

    std::string m_strText;
    QRRectImg *m_qrcode;
};

#endif // QRFILTER_H
