#include "videorender.h"
#include <QPainter>
#include <QTime>
#include <QEvent>
#include <QRegion>
#include <QCoreApplication>

VideoRender::VideoRender(QWidget *parent)
    : QWidget(parent)
	, m_pDisplayImg(NULL)
	, m_pCacheImg(NULL)
{
}

VideoRender::~VideoRender()
{

}

unsigned char* VideoRender::getVideoFrameBuffer()
{
	if (m_pCacheImg)
		return m_pCacheImg->bits();
	return NULL;
}

void VideoRender::setVideoInfo(int &nWidth, int &nHeight, int &nFormat)
{
	if (m_pCacheImg)
	{
		delete m_pCacheImg;
		m_pCacheImg = NULL;
	}
	if (m_pDisplayImg)
	{
		delete m_pDisplayImg;
		m_pDisplayImg = NULL;
	}

	m_pCacheImg = new QImage(nWidth, nHeight, QImage::Format_RGBA8888);
	m_pDisplayImg = new QImage(nWidth, nHeight, QImage::Format_RGBA8888);
	m_pCacheImg->fill(Qt::black);
	m_pDisplayImg->fill(Qt::black);
}

void VideoRender::createVideoFrameOver()
{
	QImage *pTempImg = m_pCacheImg;
	m_mutex.lock();
	m_pCacheImg = m_pDisplayImg;
	m_pDisplayImg = pTempImg;
    m_mutex.unlock();
    updateUi();
}

void VideoRender::clearRenderer()
{
    if(!m_pCacheImg)
        return;
    m_pCacheImg->fill(Qt::black);
    createVideoFrameOver();
}

void VideoRender::paintEvent(QPaintEvent *event)
{
    if(!m_pDisplayImg)
    {
        QWidget::paintEvent(event);
        return ;
    }
    QPainter painter(this);
    QSize size = this->size();
    m_mutex.lock();
    QImage img = *m_pDisplayImg;
    m_mutex.unlock();

    int nWidth = size.width();
    int nHeight = size.height();
    QSizeF image = img.size();
    QSizeF ImgD;
    //宽度与高度的比例大于或等于要求显示的比例（说明比较宽）
    if (image.width()/(double)image.height() >= nWidth/(double)nHeight) {
        //如果宽度大于要求显示的宽度
        ImgD.setWidth(nWidth); //那么图片就显示要显示的宽度
        ImgD.setHeight((image.height() * nWidth) / (double)image.width());
    }
    else {
        //说明比较高
        ImgD.setHeight(nHeight);
        ImgD.setWidth((image.width() * nHeight) / (double)image.height());
    }

    QRect rect;
    rect.setTopLeft(QPoint((nWidth-ImgD.width())/2, (nHeight-ImgD.height())/2));
    rect.setWidth(ImgD.width());
    rect.setHeight(ImgD.height());
    painter.drawImage(rect, img, img.rect());
    QWidget::paintEvent(event);
}

void VideoRender::updateUi()
{
    QObject *obj = (QObject*)this;
    if (obj) {
        // UpdateRequest only sync backing store but do not shedule repainting. UpdateLater does
        // Copy from qwidget_p.h. QWidget::event() will convert UpdateLater to QUpdateLaterEvent and get it's region()
        class QUpdateLaterEvent : public QEvent
        {
        public:
            explicit QUpdateLaterEvent(const QRegion& paintRegion)
                : QEvent(UpdateLater), m_region(paintRegion)
            {}
            ~QUpdateLaterEvent() {}
            inline const QRegion &region() const { return m_region; }
        protected:
            QRegion m_region;
        };
        QCoreApplication::instance()->postEvent(obj, new QUpdateLaterEvent(QRegion(0, 0, this->width(), this->height())));
    } else {
        obj = (QObject*)this;
        if (obj)
            QCoreApplication::instance()->postEvent(obj, new QEvent(QEvent::UpdateRequest));
    }
}

