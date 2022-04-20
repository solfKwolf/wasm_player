#ifndef QRCODETOIMG_H
#define QRCODETOIMG_H
#include <iostream>
#include "qrfilter.h"

class QRCodeToImg
{
public:
    QRCodeToImg();
    ~QRCodeToImg();

    void setText(std::string strText);
    void addQRcode(unsigned char* pImg, QRFilter::ImgFormat imgFmt, QRFilter::ImgRect rect);
private:
    int generateRandom(int min, int max);

    QRFilter m_qrfilter;

    std::string m_strQRcodeText;

    int m_nQRCodeX;
    int m_nQRCodeY;
    int m_nDirectionX;
    int m_nDirectionY;
};

#endif // QRCODETOIMG_H
