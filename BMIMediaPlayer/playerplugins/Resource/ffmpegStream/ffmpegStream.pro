#-------------------------------------------------
#
# Project created by QtCreator 2016-08-08T09:34:11
#
#-------------------------------------------------

QT       -= gui core

TARGET = ffmpegStream
TEMPLATE = lib
CONFIG += c++11

DEFINES += STREAM_LIBRARY

INCLUDEPATH += \
    $$PWD/../../../interfaces

SOURCES += \
    guids.cpp \
    streamffmpeg.cpp

HEADERS += \
    streamffmpeg.h

unix
{
    CONFIG(debug, debug|release) {
        DESTDIR = $(MEDIAPLAYER_BIN)/debug
    } else {
        DESTDIR = $(MEDIAPLAYER_BIN)/release
    }
}

win32
{
    Debug:DESTDIR = $(MEDIAPLAYER_BIN)/debug
    Release:DESTDIR = $(MEDIAPLAYER_BIN)/release
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}
