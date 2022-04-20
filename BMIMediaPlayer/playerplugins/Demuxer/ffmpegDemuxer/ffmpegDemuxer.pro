#-------------------------------------------------
#
# Project created by QtCreator 2016-08-08T09:34:11
#
#-------------------------------------------------

QT       -= gui core

TARGET = ffmpegDemuxer
TEMPLATE = lib
CONFIG += c++11

DEFINES += DEMUXER_LIBRARY

INCLUDEPATH += \
    $$PWD/../../../interfaces \
    $(FFMPEG_ROOT)/include

SOURCES += \
    demuxerffmpeg.cpp \
    guids.cpp

HEADERS += \
    demuxerffmpeg.h

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

LIBS += -L$(FFMPEG_ROOT)/lib -lavcodec -lavformat -lavdevice -lavutil -lswscale -lswresample

unix {
    target.path = /usr/lib
    INSTALLS += target
}
