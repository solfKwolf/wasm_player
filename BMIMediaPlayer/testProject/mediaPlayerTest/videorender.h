#ifndef VIDEORENDER_H
#define VIDEORENDER_H

#include <QWidget>
#include <QImage>
#include <QMutex>

class VideoRender
        : public QWidget

{
    Q_OBJECT
public:
    explicit VideoRender(QWidget *parent = 0);
    ~VideoRender();

    unsigned char* getVideoFrameBuffer();
    void setVideoInfo(int &nWidth, int &nHeight, int &nFormat);
    void createVideoFrameOver();
    void clearRenderer();

protected:
    void paintEvent(QPaintEvent *event);

    void updateUi();
signals:

private:
	QImage *m_pDisplayImg;
	QImage *m_pCacheImg;
    QMutex m_mutex;
};

#endif // VIDEORENDER_H
