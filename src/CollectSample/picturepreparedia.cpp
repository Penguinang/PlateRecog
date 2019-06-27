#include "picturepreparedia.h"
#include "ui_picturepreparedia.h"
#include <QtDebug>

PicturePrepareDia::PicturePrepareDia(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PicturePrepareDia)
{
    ui->setupUi(this);

    QAction* actionSelectDir = new QAction(tr("选择路径"),this);

    this->connect(actionSelectDir,SIGNAL(triggered()),this,SLOT(selectDir()));



    this->setWindowState(Qt::WindowState::WindowMaximized);

}

PicturePrepareDia::~PicturePrepareDia()
{
    delete ui;
}
