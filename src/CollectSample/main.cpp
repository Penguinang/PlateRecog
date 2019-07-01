#include "mainwindow.h"
#include "manualclassifywindow.h"
#include "trainingfrontend.h"
#include <QApplication>
#include <QtGui>
#include <QSplashScreen>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QFile file("../../resource/style.qss");
    file.open(QFile::ReadOnly);
    QString qss=file.readAll();
    a.setStyleSheet(qss);//界面风格
    //w.setStyleSheet(qss);//试试

    //显示启动页面
    QPixmap pixmap("../../resource/icons/start01.png");
    QSplashScreen splash(pixmap);
    splash.show();

    QTimer::singleShot(1500, &splash, SLOT(hide()));
    QTimer::singleShot(2000, &w, SLOT(show()));

    //w.show();

    return a.exec();

}
