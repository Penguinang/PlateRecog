#include "manualclassifywindow.h"
#include "ui_manualclassifywindow.h"



ManualClassifyWindow::ManualClassifyWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::ManualClassifyWindow)
{
    ui->setupUi(this);
}


ManualClassifyWindow::~ManualClassifyWindow()
{
    delete ui;
}
