#include "qrcodetoimg.h"
#include <random>
#include <time.h>

QRCodeToImg::QRCodeToImg()
{

}

QRCodeToImg::~QRCodeToImg()
{

}

void QRCodeToImg::setText(std::string strText)
{
    m_strQRcodeText = strText;
}

void QRCodeToImg::addQRcode(unsigned char *pImg, QRFilter::ImgFormat imgFmt, QRFilter::ImgRect rect)
{
    if(m_strQRcodeText.empty())
        return;
    int qrCodeWidth = rect.width * 0.05;
    if((m_nQRCodeX == 0 && m_nQRCodeY == 0) ||
            m_nQRCodeX < 0 || m_nQRCodeY < 0 ||
            m_nQRCodeX + qrCodeWidth > rect.width || m_nQRCodeY + qrCodeWidth > rect.height)
    {
        m_nQRCodeX = generateRandom(0, rect.width - qrCodeWidth);
        m_nQRCodeY = generateRandom(0, rect.height - qrCodeWidth);
        m_nDirectionX = (m_nQRCodeX > (rect.width - qrCodeWidth)/2 ? -1 : 1);
        m_nDirectionY = (m_nQRCodeY > (rect.height - qrCodeWidth)/2 ? -1 : 1);
    }
    QRFilter::ImgRect qrRect{m_nQRCodeX, m_nQRCodeY, qrCodeWidth, qrCodeWidth};

    m_qrfilter.addQRCode(pImg, imgFmt, rect, m_strQRcodeText, qrRect);

    m_nQRCodeX += (rect.width/250*m_nDirectionX);
    m_nQRCodeY += (rect.height/250*m_nDirectionY);
}

int QRCodeToImg::generateRandom(int min, int max)
{
    std::default_random_engine generator(time(nullptr)*time(nullptr));
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

