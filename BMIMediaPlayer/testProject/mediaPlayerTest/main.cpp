#include <QImage>
#include <QApplication>
#include <iostream>
#include <packet.h>
#include <demux.h>
#include <decode.h>
#include <frame.h>
#include <scale.h>
#include <fstream>
#include <play.h>
#include <vector>
#include "comdelegate.h"
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
