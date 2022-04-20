#-------------------------------------------------
#
# Project created by QtCreator 2016-08-08T09:34:11
#
#-------------------------------------------------

#QT       -= gui
QT       += gui
QT       += multimedia

TARGET = ffmpegDecoderTest
TEMPLATE = app
CONFIG += c++11


INCLUDEPATH += \
    $$PWD/../../interfaces \
    $$PWD/../../util

SOURCES += \
    main.cpp \
    guids.cpp \
    audiorender.cpp \
    cbasethread.cpp \
    decoder.cpp
HEADERS += \
    audiorender.h \
    cbasethread.h \
    decoder.h

unix
{
    CONFIG(debug, debug|release) {
        DESTDIR = $(MEDIAPLAYER_BIN)/debug

    } else {
        DESTDIR = $(MEDIAPLAYER_BIN)/release
    }
#    LIBS += -ldl
}

win32
{
    Debug:DESTDIR = $(MEDIAPLAYER_BIN)/debug
    Release:DESTDIR = $(MEDIAPLAYER_BIN)/release
}


LIBS += -L$(DESTDIR) -lutil

LIBS += -L$$PWD/../../install/lib -ljpeg


unix {
    target.path = /usr/lib
    INSTALLS += target
}
