#-------------------------------------------------
#
# Project created by QtCreator 2017-08-02T16:32:16
#
#-------------------------------------------------

QT += core gui
QT += multimedia

TARGET = MediaPlayer
CONFIG   += c++11
TEMPLATE = lib

DEFINES +=  MEDIAPLAYER_LIBRARY HAVE_CONFIG_H
#QMAKE_CXXFLAGS += \
#    -D_GLIBCXX_USE_CXX11_ABI=0

INCLUDEPATH += \
        $$PWD/../util \
        $$PWD/../interfaces \
        $$PWD/../util/boost

DEFINES += __STDC_CONSTANT_MACROS NDEBUG

SOURCES += \
    guids.cpp

unix
{
    CONFIG(debug, debug|release) {
        LIBS += -L$(MEDIAPLAYER_BIN)/debug -lutil
        DESTDIR = $(MEDIAPLAYER_BIN)/debug
    } else {
        LIBS += -L$(MEDIAPLAYER_BIN)/release -lutil
        DESTDIR = $(MEDIAPLAYER_BIN)/release
    }
}

win32
{
    Debug: LIBS += -L$(MEDIAPLAYER_BIN)/debug -lutil
    Release: LIBS += -L$(MEDIAPLAYER_BIN)/release -lutil
    Debug:DESTDIR = $(MEDIAPLAYER_BIN)/debug
    Release:DESTDIR = $(MEDIAPLAYER_BIN)/release
}
include(Thread.pri)
include(Player.pri)
include(CBaseThread.pri)
include(Common.pri)
include(qrcode.pri)








