#include "mainwindow.h"
#include "manualclassifywindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
   // ManualClassifyWindow w;
   // w.show();

    return a.exec();
}
