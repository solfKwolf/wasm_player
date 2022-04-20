#include "WebGLYUVRender.h"
#include "frame.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#include <mutex>

#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4

using namespace emscripten;

static const char *vertexShader = "\
attribute vec4 vertexIn;\
attribute vec2 textureIn;\
varying vec2 textureOut;\
uniform mat4 mWorld;\
uniform mat4 mView;\
uniform mat4 mProj;\
void main(void)\
{\
    gl_Position =vertexIn * mWorld * mView * mProj  ;\
    textureOut = textureIn;\
}";

static const char *fragmentShader = "\
#ifdef GL_ES\n  \
precision mediump float;\n \
#endif\n \
varying vec2 textureOut;\
uniform sampler2D tex_y;\
uniform sampler2D tex_u;\
uniform sampler2D tex_v;\
void main(void)\
{\
    vec3 yuv;\
    vec3 rgb;\
    yuv.x = texture2D(tex_y, textureOut).r;\
    yuv.y = texture2D(tex_u, textureOut).r - 0.5;\
    yuv.z = texture2D(tex_v, textureOut).r - 0.5;\
    rgb = mat3( 1,       1,         1,\
                0,       -0.39465,  2.03211,\
                1.13983, -0.58060,  0) * yuv;\
    gl_FragColor = vec4(rgb, 1);\
}";

static const GLfloat vertexVertices[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f,  1.0f,
};

static const GLfloat textureVertices[] = {
    0.0f,  1.0f,
    1.0f,  1.0f,
    0.0f,  0.0f,
    1.0f,  0.0f,
};

static GLfloat world[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

static GLfloat view[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

static GLfloat proj[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};


WebGLYUVRender::WebGLYUVRender(std::string target,int nWidth,int nHeight)
        :m_target(target)
{
    initializeWebGL(nWidth,nHeight);
}

WebGLYUVRender::~WebGLYUVRender()
{
}

void WebGLYUVRender::initializeWebGL(int nWidth,int nHeight)
{   
    emscripten_set_canvas_element_size(m_target.c_str(),nWidth,nHeight);
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = attr.depth = attr.stencil = attr.antialias = attr.preserveDrawingBuffer = attr.failIfMajorPerformanceCaveat = 0;
    attr.enableExtensionsByDefault = 1;
    attr.premultipliedAlpha = 0;
    attr.majorVersion = 1;
    attr.minorVersion = 0;
    m_ctx = emscripten_webgl_create_context(m_target.c_str(), &attr);
    InitShaders();
}

void WebGLYUVRender::setCanvasSize(int width,int height)
{
    emscripten_webgl_make_context_current(m_ctx);
    emscripten_set_canvas_element_size(m_target.c_str(),width,height);
}

void WebGLYUVRender::setFormat(int nFormat)
{
    nFormat = IFrame::FormatYUV420P;
}

int WebGLYUVRender::getFormat() 
{
    return IFrame::FormatYUV420P;
}

void WebGLYUVRender::drawYUV(std::string imgData,int width,int height) 
{
    paintGL((unsigned char*)imgData.c_str(), width, height);
}

void WebGLYUVRender::clearRenderer(){
    glClearColor(0.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void WebGLYUVRender::InitShaders()
{
    emscripten_webgl_make_context_current(m_ctx);

    GLint vertCompiled, fragCompiled, linked;
    GLint v, f;

    //Shader: step1
    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);

    //Shader: step2
    glShaderSource(v, 1, &vertexShader,NULL);
    glShaderSource(f, 1, &fragmentShader,NULL);

    //Shader: step3
    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &vertCompiled);    //Debug

    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &fragCompiled);    //Debug

    //Program: Step1
    m_program = glCreateProgram();
    //Program: Step2
    glAttachShader(m_program,v);
    glAttachShader(m_program,f);

    glBindAttribLocation(m_program, ATTRIB_VERTEX, "vertexIn");
    glBindAttribLocation(m_program, ATTRIB_TEXTURE, "textureIn");
    //Program: Step3
    glLinkProgram(m_program);
    //Debug
    glGetProgramiv(m_program, GL_LINK_STATUS, &linked);

    glUseProgram(m_program);

    glGenBuffers(1, &m_vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboVertex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexVertices), vertexVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_vboTexture);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboTexture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices), textureVertices, GL_STATIC_DRAW);

    //Get Uniform Variables Location
    m_sampler_y = glGetUniformLocation(m_program, "tex_y");
    m_sampler_u = glGetUniformLocation(m_program, "tex_u");
    m_sampler_v = glGetUniformLocation(m_program, "tex_v");

    //Init Texture
    glGenTextures(1, &m_tex_y);
    glBindTexture(GL_TEXTURE_2D, m_tex_y);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &m_tex_u);
    glBindTexture(GL_TEXTURE_2D, m_tex_u);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &m_tex_v);
    glBindTexture(GL_TEXTURE_2D, m_tex_v);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindBuffer(GL_ARRAY_BUFFER, m_vboVertex);
    glVertexAttribPointer(ATTRIB_VERTEX,2,GL_FLOAT,GL_FALSE, 2 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    glBindBuffer(GL_ARRAY_BUFFER, m_vboTexture);
    glVertexAttribPointer(ATTRIB_TEXTURE,2,GL_FLOAT,GL_FALSE, 2 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);

    m_matWorld = glGetUniformLocation(m_program,"mWorld");
    m_matView = glGetUniformLocation(m_program,"mView");
    m_matProj = glGetUniformLocation(m_program,"mProj");
}

void WebGLYUVRender::paintGL(unsigned char* pImgData,int w,int h) 
{
    // 清除缓冲区
    emscripten_webgl_make_context_current(m_ctx);

    glClearColor(0.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint vboVertex;
    GLfloat imgWidth = (GLfloat)w;
    GLfloat imgHeight = (GLfloat)h;
    
    int canvasW,canvasH;
    emscripten_get_canvas_element_size(m_target.c_str(),&canvasW,&canvasH);

    GLfloat canvasScale = canvasW / canvasH;
    GLfloat imgScale = imgWidth / imgHeight;
    
    bool enableWidthScale = (imgScale > canvasScale);
    if(enableWidthScale) {
        GLfloat heightScale = imgHeight / (imgWidth / canvasW) / canvasH;
        GLfloat vertexVer[] = {-1.0f, -heightScale, 1.0f, -heightScale, -1.0f, heightScale, 1.0f, heightScale};
        glGenBuffers(1, &vboVertex);
        glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexVer), vertexVer, GL_STATIC_DRAW);
        glVertexAttribPointer(ATTRIB_VERTEX,2,GL_FLOAT,GL_FALSE, 2 * sizeof(GLfloat), 0);
    } else {
        GLfloat widthScale = imgWidth / (imgHeight/canvasH) / canvasW;
        GLfloat vertexVer[] = {-widthScale, -1.0f, widthScale, -1.0f, -widthScale, 1.0f, widthScale, 1.0f};
        glGenBuffers(1, &vboVertex);
        glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexVer), vertexVer, GL_STATIC_DRAW);
        glVertexAttribPointer(ATTRIB_VERTEX,2,GL_FLOAT,GL_FALSE, 2 * sizeof(GLfloat), 0);
    }

    // Y
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex_y);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (GLvoid*)pImgData);
    glUniform1i(m_sampler_y, 0);

    // U
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_tex_u);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w/2, h/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (GLvoid*)(pImgData + w*h));
    glUniform1i(m_sampler_u, 1);

    // V
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_tex_v);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w/2, h/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (GLvoid*)(pImgData + w*h + w*h/4));
    glUniform1i(m_sampler_v, 2);

//        QOpenGLShaderProgram::setUniformValue();
    glUniformMatrix4fv(m_matWorld,1, GL_FALSE,world);
    glUniformMatrix4fv(m_matView,1, GL_FALSE,view);
    glUniformMatrix4fv(m_matProj,1, GL_FALSE,proj);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glFlush();
}


EMSCRIPTEN_BINDINGS(WebGLYUVRender){
    class_<WebGLYUVRender>("WebGLYUVRender")
       .constructor<std::string,int,int>()
       .function("setFormat", &WebGLYUVRender::setFormat)
       .function("getFormat",&WebGLYUVRender::getFormat)
       .function("drawYUV", &WebGLYUVRender::drawYUV)
       .function("clearRenderer", &WebGLYUVRender::clearRenderer)
       ;
}


