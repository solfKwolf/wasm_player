#ifndef GLYUVRENDER_H
#define GLYUVRENDER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_2_0>
#include <QMatrix4x4>
#include <QMutex>

class I420Image
{
public:
    I420Image(int w, int h)
        : width(w)
        , height(h)
    {
        data = new uint8_t[w*h + w*h / 2];
        memset(data, 0x00, w*h);
        memset(GetU(), 0x80, w*h/4);
        memset(GetV(), 0x80, w*h/4);
    }
    ~I420Image()
    {
        delete []data;
    }

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    uint8_t *GetY() const { return data; }
    uint8_t *GetU()const { return data + width*height; }
    uint8_t *GetV()const { return data + width*height + width*height / 4; }
protected:
private:
    int width;
    int height;
    uint8_t * data;
};



class GLYUVRender : public QOpenGLWidget , public QOpenGLFunctions_2_0
{
    Q_OBJECT
public:
    explicit GLYUVRender(QWidget *parent = 0);
    ~GLYUVRender();

    unsigned char* getVideoFrameBuffer();
    void setVideoInfo(int &nWidth, int &nHeight, int &nFormat);
    void createVideoFrameOver();
    void clearRenderer();

signals:

public slots:

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;
    void updateUi();

private:
    void InitShaders();
    GLuint program;
    GLuint tex_y, tex_u, tex_v;
    GLuint sampler_y, sampler_u, sampler_v;
    GLuint matWorld, matView, matProj;
    QMatrix4x4 mProj;
    QMatrix4x4 mView;
    QMatrix4x4 mWorld;
    QMutex m_imgMutex;

private:
    I420Image *m_Front;
    I420Image *m_Back;
};
#endif // GLYUVRENDER_H
