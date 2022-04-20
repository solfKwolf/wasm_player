#-------------------------------------------------
#
# Project created by QtCreator 2016-08-08T09:34:11
#
#-------------------------------------------------

#QT       -= gui
QT       += multimedia

TARGET = AITrainDecoder
TEMPLATE = lib
CONFIG += c++11

QMAKE_CXXFLAGS += \
    -D_GLIBCXX_USE_CXX11_ABI=0

INCLUDEPATH += \
    $$PWD/../../interfaces \
    $$PWD/../../util \
    $$PWD/../../install/include

SOURCES += \
    guids.cpp \
    aitraindecoder.cpp \
    savepicture.cpp

HEADERS += \
    aitraindecoder.h \
    savepicture.h

unix
{
    CONFIG(debug, debug|release) {
        DESTDIR = $(MEDIAPLAYER_BIN)/debug
        LIBS += -L$(MEDIAPLAYER_BIN)/debug -lutil
    } else {
        DESTDIR = $(MEDIAPLAYER_BIN)/release
        LIBS += -L$(MEDIAPLAYER_BIN)/release -lutil
    }
}

win32
{
    Debug:DESTDIR = $(MEDIAPLAYER_BIN)/debug
    Release:DESTDIR = $(MEDIAPLAYER_BIN)/release
}
#LIBS += -L$$PWD/../../util/boost/lib \
#    -lboost_filesystem -lboost_system

#LIBS += -L$(DESTDIR) -lutil
LIBS += -L/home/user/qt-workspace/BMIDarwin/lib/boost/x64/sharedLib -lboost_filesystem -lboost_system
LIBS += -L$$PWD/../../install/lib -ljpeg

unix {
    target.path = /usr/lib
    INSTALLS += target
}
