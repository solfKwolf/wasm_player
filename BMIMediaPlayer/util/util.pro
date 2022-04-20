#-------------------------------------------------
#
# Project created by QtCreator 2016-08-08T09:34:11
#
#-------------------------------------------------

QT       -= gui core

TARGET = util
TEMPLATE = lib
#CONFIG += staticlib
CONFIG += c++11
#QMAKE_CXXFLAGS += \
#    -D_GLIBCXX_USE_CXX11_ABI=0

DEFINES += UTIL_LIBRARY

INCLUDEPATH += \
    $$PWD/../interfaces \
    $$PWD/boost

SOURCES += \
    comdelegate.cpp \

HEADERS += \
    util_global.h \
    comdelegate.h \

win32
{
    Debug:DESTDIR = $(MEDIAPLAYER_BIN)/debug
    Release:DESTDIR = $(MEDIAPLAYER_BIN)/release
}

unix
{
    CONFIG(debug, debug|release) {
        DESTDIR = $(MEDIAPLAYER_BIN)/debug

    } else {
        DESTDIR = $(MEDIAPLAYER_BIN)/release
    }
}

LIBS += -L$$(BOOST_LIBRARY) -lboost_filesystem -lboost_system

unix {
    target.path = /usr/lib
    INSTALLS += target
}
