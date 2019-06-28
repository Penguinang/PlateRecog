#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>
#include <QFileInfo>
#include <QImage>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>

#include "../classifier/PlateCategory_SVM.h"
#include "../classifier/PlateChar_SVM.h"
#include "../classifier/PlateRecognition_V3.h"

using namespace Doit::CV::PlateRecogn;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->state = false;
    this->currentImagePath = tr("");
    //this->plateModelPath = tr("");
    //this->charModelPath = tr("");

    QIcon selectDir(":/icon/icon/wenjian.png");
    QPushButton* buttonSelectDir = new QPushButton(selectDir,tr("选择测试图像"));
    buttonSelectDir->setStyleSheet( "QPushButton{background: transparent;}"
                                    "QPushButton:hover{background:gray;}");
    this->ui->toolBar->addWidget(buttonSelectDir);
    this->connect(buttonSelectDir,SIGNAL(clicked()),this,SLOT(selectDir()));

    this->ui->toolBar->addSeparator();

    QIcon selectModel(":/icon/icon/daoru.png");

    QPushButton* buttonSelecPlatetModel = new QPushButton(selectModel,tr("导入车牌识别模型"));
    buttonSelecPlatetModel->setStyleSheet( "QPushButton{background: transparent;}"
                                      "QPushButton:hover{background:gray;}");
    this->connect(buttonSelecPlatetModel,SIGNAL(clicked()),this,SLOT(selectPlateModel()));
    this->ui->toolBar->addWidget(buttonSelecPlatetModel);

    this->ui->toolBar->addSeparator();

    QPushButton* buttonSelectCharModel = new QPushButton(selectModel,tr("导入字符识别模型"));
    buttonSelectCharModel->setStyleSheet( "QPushButton{background: transparent;}"
                                      "QPushButton:hover{background:gray;}");
    this->connect(buttonSelectCharModel,SIGNAL(clicked()),this,SLOT(selectCharModel()));
    this->ui->toolBar->addWidget(buttonSelectCharModel);

    this->ui->toolBar->addSeparator();

    QIcon recog(":/icon/icon/tableactivation.png");
    QPushButton* buttonRecog = new QPushButton(recog,tr("识别"));
    buttonRecog->setStyleSheet( "QPushButton{background: transparent;}"
                                      "QPushButton:hover{background:gray;}");
    this->connect(buttonRecog,SIGNAL(clicked()),this,SLOT(recog()));
    this->ui->toolBar->addWidget(buttonRecog);

    this->ui->verticalLayout->setStretchFactor(this->ui->plateWidget,2);
    this->ui->verticalLayout->setStretchFactor(this->ui->charWidget,3);

    this->setWindowState(Qt::WindowState::WindowMaximized);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::recog()
{
    if(this->plateModelPath == "")
    {
        QMessageBox::information(NULL, "提示", "请先导入车牌识别模型",
                                 QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    if(this->charModelPath == "")
    {
        QMessageBox::information(NULL, "提示", "请先导入字符识别模型",
                                 QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    QFileInfo imageInfo(this->currentImagePath);
    QFileInfo plateModel(this->plateModelPath);
    QFileInfo charModel(this->charModelPath);
    if(imageInfo.exists() == false)
    {
        QMessageBox::information(NULL, "提示", "无效的图像",
                                 QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    if(plateModel.exists() == false)
    {
        QMessageBox::information(NULL, "提示", "无效的车牌识别模型",
                                 QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    if(charModel.exists() == false)
    {
        QMessageBox::information(NULL, "提示", "无效的字符识别模型",
                                 QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    initSvm(this->plateModelPath,this->charModelPath);
    cv::Mat image = cv::imread(this->currentImagePath.toLocal8Bit().toStdString());
    auto plateInfos = PlateRecognition_V3::Recognite(image);
    this->ui->plateWidget->clear();
    for(int i=1;i<=plateInfos.size();i++)
    {
        PlateInfo plateInfo = plateInfos[i-1];
        this->generatePlateRegion(plateInfo,i);
    }
}
void MainWindow::generateCharRegion(PlateInfo plateInfo,int index)
{
    vector<CharInfo> charInfos = plateInfo.CharInfos;
    if(charInfos.size()==0)
    {
        return;
    }
    QTableWidget * tableCharInfo = new QTableWidget;
    tableCharInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableCharInfo->setSelectionMode(QAbstractItemView::NoSelection);
    tableCharInfo->verticalHeader()->setVisible(false);
    tableCharInfo->horizontalHeader()->setVisible(false);

    tableCharInfo->setRowCount(8);
    tableCharInfo->setColumnCount(6);

    // verticalHeader
    tableCharInfo->setItem(0,1,new QTableWidgetItem(tr("字符")));
    tableCharInfo->setItem(0,2,new QTableWidgetItem(tr("宽度")));
    tableCharInfo->setItem(0,3,new QTableWidgetItem(tr("高度")));
    tableCharInfo->setItem(0,4,new QTableWidgetItem(tr("宽高比")));
    tableCharInfo->setItem(0,5,new QTableWidgetItem(tr("切分方法")));

    for(int i=1;i <= charInfos.size();i++)
    {
        CharInfo charInfo = charInfos[i-1];
        cv::Mat charMatClone = charInfo.OriginalMat.clone();
        if(charInfo.OriginalMat.channels()==3)
        {
            cv::cvtColor(charMatClone,charMatClone,cv::COLOR_BGR2RGB);
        }
        QImage image = QImage((const unsigned char*)(charMatClone.data),
                              charMatClone.cols,
                              charMatClone.rows,
                              charMatClone.cols * charMatClone.channels(),
                              QImage::Format_RGB888);
        qDebug()<<"image " << image.size();
        image = image.scaled(image.width()*3,image.height()*3,Qt::IgnoreAspectRatio);
        QPixmap pixmap = QPixmap::fromImage(image);
        QIcon charIcon(pixmap);
        tableCharInfo->setIconSize(QSize(image.width()*3/2,image.height()*3/2));
        tableCharInfo->setItem(i,0,new QTableWidgetItem(charIcon,tr("")));

        tableCharInfo->setItem(i,1,new QTableWidgetItem(charInfo.ToString().c_str()));

        tableCharInfo->setItem(i,2,new QTableWidgetItem(QString::number(charInfo.OriginalRect.width)));
        tableCharInfo->setItem(i,3,new QTableWidgetItem(QString::number(charInfo.OriginalRect.height)));

        ostringstream buffer;
        buffer.str("");
        buffer << std::setiosflags(ios::fixed)
               << std::setprecision(2)
               << float(charInfo.OriginalRect.width) / charInfo.OriginalRect.height;
        tableCharInfo->setItem(i,4,new QTableWidgetItem(tr(buffer.str().c_str())));

        buffer.str("");
        buffer << charInfo.CharSplitMethod;
        tableCharInfo->setItem(i,5,new QTableWidgetItem(tr(buffer.str().c_str())));
    }


    for(int j=0;j<tableCharInfo->columnCount()-2;j++)
    {
        tableCharInfo->setColumnWidth(j,40);
    }
    tableCharInfo->setColumnWidth(tableCharInfo->columnCount()-2,60);
    tableCharInfo->setColumnWidth(tableCharInfo->columnCount()-1,80);

    for(int k=0;k<tableCharInfo->rowCount();k++)
    {
        tableCharInfo->setRowHeight(k,65);
    }
    this->ui->charWidget->clear();
    this->ui->charWidget->addTab(tableCharInfo,tr("字符信息")+QString::number(index));
}
void MainWindow::generatePlateRegion(PlateInfo plateInfo,int index)
{
    this->ui->plateWidget->clear();
    QWidget* plateRegion = new QWidget(this);
    QLabel* labelPlateImage = new QLabel;

    cv::Mat matClone = plateInfo.OriginalMat.clone();
    if(plateInfo.OriginalMat.channels()==3)
    {
        cv::cvtColor(matClone,matClone,cv::COLOR_BGR2RGB);
    }
    QImage image = QImage((const unsigned char*)(matClone.data),
                          matClone.cols,
                          matClone.rows,
                          matClone.cols*matClone.channels(),
                          QImage::Format_RGB888);

    image = image.scaled(image.width()*3/2,image.height()*3/2,Qt::IgnoreAspectRatio);
    qDebug()<<image.size();
    QPixmap pixmap = QPixmap::fromImage(image);
    qDebug()<<pixmap.size();
    labelPlateImage->setAlignment(Qt::AlignCenter);
    labelPlateImage->setPixmap(pixmap);
    labelPlateImage->resize(pixmap.size());
    qDebug()<<labelPlateImage->size();
    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(labelPlateImage);

    QTableWidget* tablePlateInfo = new QTableWidget();
    tablePlateInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablePlateInfo->setSelectionMode(QAbstractItemView::NoSelection);
    tablePlateInfo->verticalHeader()->setVisible(false);
    tablePlateInfo->horizontalHeader()->setVisible(false);

    tablePlateInfo->setRowCount(6);
    tablePlateInfo->setColumnCount(2);

    QIcon recog(":/icon/icon/plate.png");
    tablePlateInfo->setItem(0,0,new QTableWidgetItem(recog,tr("车牌号")));
    tablePlateInfo->setItem(0,1,new QTableWidgetItem(tr(plateInfo.ToString().c_str())));

    QIcon cate(":/icon/icon/cate.png");
    tablePlateInfo->setItem(1,0,new QTableWidgetItem(cate,tr("车牌类别")));
    ostringstream buffer;
    buffer << plateInfo.PlateCategory;
    tablePlateInfo->setItem(1,1,new QTableWidgetItem(tr(buffer.str().c_str())));

    QIcon iconWidth(":/icon/icon/kuandu.png");
    tablePlateInfo->setItem(2,0,new QTableWidgetItem(iconWidth,tr("宽度")));
    tablePlateInfo->setItem(2,1,new QTableWidgetItem(QString::number(plateInfo.OriginalRect.width)));

    QIcon iconHeight(":/icon/icon/gaodu.png");
    tablePlateInfo->setItem(3,0,new QTableWidgetItem(iconHeight,tr("宽度")));
    tablePlateInfo->setItem(3,1,new QTableWidgetItem(QString::number(plateInfo.OriginalRect.height)));

    QIcon originalRect(":/icon/icon/rect.png");
    tablePlateInfo->setItem(4,0,new QTableWidgetItem(originalRect,tr("长宽比")));
    buffer.str("");
    buffer << std::setiosflags(ios::fixed)
           << std::setprecision(2)
           << float(plateInfo.OriginalRect.width) / plateInfo.OriginalRect.height;
    tablePlateInfo->setItem(4,1,new QTableWidgetItem(tr(buffer.str().c_str())));

    QIcon method(":/icon/icon/method.png");
    tablePlateInfo->setItem(5,0,new QTableWidgetItem(method,tr("定位方法")));
    buffer.str("");
    buffer << plateInfo.PlateLocateMethod;
    tablePlateInfo->setItem(5,1,new QTableWidgetItem(tr(buffer.str().c_str())));

    vLayout->addWidget(tablePlateInfo);
    plateRegion->setLayout(vLayout);

    this->ui->plateWidget->addTab(plateRegion,tr("车牌信息")+QString::number(index));

    this->generateCharRegion(plateInfo,index);

}

void MainWindow::initSvm(QString plateModelPath,QString charModelPath)
{
    PlateCategory_SVM::Load(plateModelPath.toStdString());
    PlateChar_SVM::Load(charModelPath.toStdString());
}

void MainWindow::selectDir()
{
    this->pathSelected = QFileDialog::getExistingDirectory(this,
                                                           tr("选择测试图像文件夹"),
                                                           this->pathSelected.isEmpty()?tr("D:/download"):this->pathSelected);
    if(this->pathSelected.isEmpty()) return;

    QDir* dir = new QDir(this->pathSelected);
    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.png" << "*.bmp";
    QStringList imgFileNames = dir->entryList(nameFilters);

    for (QString imgFileName : imgFileNames)
    {
        this->ui->fileList->addItem(imgFileName);
    }
}

void MainWindow::selectPlateModel()
{
    this->plateModelPath = QFileDialog::getOpenFileName(
                this,
                tr("Open model"),
                this->plateModelPath.isEmpty()?tr("D:/download"):this->plateModelPath,
                "*.yaml");
    qDebug()<<this->plateModelPath;
}

void MainWindow::selectCharModel()
{
    this->charModelPath = QFileDialog::getOpenFileName(
                this,
                tr("Open model"),
                this->charModelPath.isEmpty()?tr("D:/download"):this->charModelPath,
                "*.yaml");
    qDebug()<<this->charModelPath;
}

void MainWindow::imageArea(cv::Mat mat)
{
    this->ui->tabWidget->clear();

    QWidget* tabOriginal = this->generateImageLabel(mat,QImage::Format_RGB888);
    this->ui->tabWidget->addTab(tabOriginal,tr("测试图像"));

    this->ui->textEdit->clear();
    QFileInfo file(this->currentImagePath);

    QString fileInfo(tr("路径: ") + this->currentImagePath +
                     tr("\n大小: ") + QString::number(file.size()) + tr("B") +
                     tr("\n像素: ") + QString::number(mat.cols) +tr(" * ") + QString::number(mat.rows));

    this->ui->textEdit->setText(fileInfo);
    this->ui->textEdit->setReadOnly(true);
}


QLabel* MainWindow::generateImageLabel(cv::Mat mat,QImage::Format format)
{
    QLabel* lblImage = new QLabel();

    cv::Mat matClone = mat.clone();
    if(mat.channels()==3)
    {
        cv::cvtColor(matClone,matClone,cv::COLOR_BGR2RGB);
    }
    QImage image = QImage((const unsigned char*)(matClone.data),
                          matClone.cols,
                          matClone.rows,
                          matClone.cols*matClone.channels(),
                          format);

    QPixmap pixmap = QPixmap::fromImage(image);
    lblImage->setPixmap(pixmap);
    lblImage->resize(pixmap.size());

    return lblImage;
}

void MainWindow::on_fileList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(current == nullptr) return;

    this->ui->plateWidget->clear();
    this->ui->charWidget->clear();

    QString imgFileName = current->text();
    imgFileName = this->pathSelected + "/" + imgFileName;
    this->currentImagePath = imgFileName;
    qDebug()<<this->currentImagePath;

    cv::Mat mat = cv::imread(imgFileName.toLocal8Bit().toStdString());
    this->imageArea(mat);
    mat.release();
}
