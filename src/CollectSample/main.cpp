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
    //trainingFrontEnd tfe;

    QPixmap pixmap("../icons/start01.png");
    QSplashScreen splash(pixmap);
    splash.show();

    QTimer::singleShot(1500, &splash, SLOT(hide()));
    QTimer::singleShot(2000, &w, SLOT(show()));

    //w.show();
    //w.setStyle(QApplication::setStyle("macintosh"));
    //QApplication::setStyle("macintosh");
   // ManualClassifyWindow w;
   // w.show();
    //tfe.show();
    return a.exec();
}
