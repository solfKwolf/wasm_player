#-------------------------------------------------
#
# Project created by QtCreator 2016-08-08T09:34:11
#
#-------------------------------------------------

QT       -= gui core

TARGET = ffmpegDemuxerTest
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += \
    $$PWD/../../interfaces \
    $$PWD/../../util

SOURCES += \
    main.cpp \
    guids.cpp

HEADERS +=

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

unix {
    target.path = /usr/lib
    INSTALLS += target
}
