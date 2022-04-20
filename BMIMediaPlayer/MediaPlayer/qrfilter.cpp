#include "qrfilter.h"
extern "C"
{
#include "qrcode/qrencode.h"
}

#define _r(c)  ((c)>>24)
#define _g(c)  (((c)>>16)&0xFF)
#define _b(c)  (((c)>>8)&0xFF)
#define _a(c)  ((c)&0xFF)

#include <cmath>

QRFilter::QRFilter()
    : m_qrcode(nullptr)
{

}

QRFilter::~QRFilter()
{
    if(m_qrcode != nullptr)
    {
        delete[] m_qrcode->buffer;
        delete m_qrcode;
    }
}

void QRFilter::addQRCode(unsigned char* pImg, ImgFormat imgFmt, ImgRect rect,
                         std::string strText, ImgRect qrcodeRect)
{
    if(strText != m_strText)
    {
        if(m_qrcode != nullptr)
        {
            delete[] m_qrcode->buffer;
            delete m_qrcode;
        }
        m_strText = strText;
        m_qrcode = GenerateQRcode(m_strText, qrcodeRect.width);
    }

    if(m_qrcode == nullptr)
        return;

    imageYUV srcImg;
    srcImg.buffer = pImg;
    srcImg.height = rect.height;
    srcImg.width = rect.width;
    srcImg.qrcode_x = getMin(qrcodeRect.x, rect.width - m_qrcode->width);
    srcImg.qrcode_y = getMin(qrcodeRect.y, rect.height - m_qrcode->height);
    blend(&srcImg, m_qrcode);

}

void QRFilter::clearQRCode()
{
    m_strText = "";
    if(m_qrcode != nullptr)
    {
        delete[] m_qrcode->buffer;
        delete m_qrcode;
    }
}

void QRFilter::blend(QRFilter::imageYUV *frame, QRFilter::QRRectImg *img)
{
    unsigned char opacity = 0xff;
    unsigned char r = 0xff;
    unsigned char g = 0xff;
    unsigned char b = 0xff;

    unsigned char y = 0.257*r+0.504*g+0.098*b+16;
    unsigned char u = 0.439*r-0.368*g-0.071*b+128;
    unsigned char v = -0.148*r-0.291*g+0.439*b+128;

    unsigned char *src;
    unsigned char *dstY, *dstU, *dstV;

    src = img->buffer;
    dstY = frame->buffer + frame->qrcode_y * frame->width + frame->qrcode_x;
    dstU = frame->buffer + frame->height * frame->width + frame->qrcode_y/2 * frame->width/2 + frame->qrcode_x/2;
    dstV = frame->buffer + frame->height * frame->width*5/4 + frame->qrcode_y/2 * frame->width/2 + frame->qrcode_x/2;
    for (int i = 0; i < img->height; ++i) {
        for (int j = 0; j < img->width; ++j) {
            unsigned k = ((unsigned) src[j]) * opacity / 255;
            // possible endianness problems
            dstY[j] = (k * y + (255 - k) * dstY[j]) / 255;
            dstU[j/2] = (k * u + (255 - k) * dstU[j/2]) / 255;
            dstV[j/2] = (k * v + (255 - k) * dstV[j/2]) / 255;
        }
        src += img->width;
        dstY += frame->width;
        if(i % 2 == 0)
        {
            dstU += frame->width/2;
            dstV += frame->width/2;
        }
    }
}

QRFilter::QRRectImg *QRFilter::GenerateQRcode(std::string strText, int width)
{
    QRcode *qrcode = QRcode_encodeString(strText.c_str(), QRcodeVersion, QR_ECLEVEL_Q, QR_MODE_8, 1);
    if(qrcode == nullptr)
        return nullptr;
    int nScale = width / qrcode->width + 1;
    width = nScale * qrcode->width;
    QRRectImg *img = new QRRectImg;
    img->height = width;
    img->width = width;
    img->buffer = new unsigned char[width * width];

    for(int i = 0; i < img->width; i++)
    {
        for(int j = 0; j < img->width; j++)
        {
            int qrcodeIndex = i/nScale * qrcode->width + j/nScale;
            unsigned char data = qrcode->data[qrcodeIndex];
            if(data & 0x01 == 1)
            {
                img->buffer[i * img->width + j] = 0;
            }
            else
            {
                img->buffer[i * img->width + j] = 255;
            }
        }
    }

    QRcode_free(qrcode);
    return img;
}

