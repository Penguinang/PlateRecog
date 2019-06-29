#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QSplashScreen>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QPixmap pixmap(":/icon/icon/start.png");
    QSplashScreen splash(pixmap);
    splash.show();

    QTimer::singleShot(1500, &splash, SLOT(hide()));
    QTimer::singleShot(2000, &w, SLOT(show()));


    return a.exec();
}
