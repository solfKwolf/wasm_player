#ifndef WEBGLYUVRENDER_H
#define WEBGLYUVRENDER_H

#include <mutex>
#include <GLES2/gl2.h>
#include <emscripten/html5.h>

class WebGLYUVRender {
public:
    WebGLYUVRender(std::string target,int nWidth,int nHeight);
    ~WebGLYUVRender();

    void setCanvasSize(int width,int height);
    void setFormat(int nFormat);
    int  getFormat();
    void drawYUV(std::string imgData,int w,int h);
    void clearRenderer();

protected:  
    void paintGL(unsigned char* pImgData,int width,int height);

private:
    void initializeWebGL(int nWidth,int nHeight);
    void InitShaders();
    GLuint m_program;
    GLuint m_tex_y, m_tex_u, m_tex_v;
    GLuint m_sampler_y, m_sampler_u, m_sampler_v;
    GLuint m_matWorld, m_matView, m_matProj;

    GLuint m_vboVertex, m_vboTexture;
    std::string m_target;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_ctx;
};

#endif // GLYUVRENDER_H