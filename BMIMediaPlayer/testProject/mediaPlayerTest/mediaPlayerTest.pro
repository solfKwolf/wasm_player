#-------------------------------------------------
#
# Project created by QtCreator 2016-08-08T09:34:11
#
#-------------------------------------------------

#QT       -= gui
QT       += gui
QT       += multimedia
QT       += widgets

TARGET = mediaPlayerTest
TEMPLATE = app
CONFIG += c++11
#QMAKE_CXXFLAGS += \
#    -D_GLIBCXX_USE_CXX11_ABI=0

INCLUDEPATH += \
    $$PWD/../../interfaces \
    $$PWD/../../util

include(render.pri)
SOURCES += \
    main.cpp \
    guids.cpp \
    mainwindow.cpp \
    urlsetter.cpp \
    fileiodevice.cpp


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

FORMS += \
    mainwindow.ui \
    urlsetter.ui

HEADERS += \
    mainwindow.h \
    urlsetter.h \
    fileiodevice.h
