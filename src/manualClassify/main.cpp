#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.showImages(1);
    //w.labelImage("D:/picture/test/0NXY0_粤ZJF76港_40761.jpg",1);

    w.show();

    return a.exec();
}
