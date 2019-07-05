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
#include <QTime>

#include "../classifier/PlateCategory_SVM.h"
#include "../classifier/PlateChar_SVM.h"
#include "../classifier/PlateRecognition_V3.h"
#include "../classifier/Utilities.h"

using namespace Doit::CV::PlateRecogn;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/icon/icon/plateUI03.png"));
    this->setWindowTitle(tr("Plate Recognition [User Version]"));

    this->state = false;
    this->currentImagePath = tr("");
    this->plateModelPath = tr("");
    this->charModelPath = tr("");

    QIcon selectDir(":/icon/icon/wenjian.png");
    QPushButton* buttonSelectDir = new QPushButton(selectDir,QString::fromLocal8Bit("选择测试图像"));
    buttonSelectDir->setStyleSheet( "QPushButton{background: transparent;}"
                                    "QPushButton:hover{background:gray;}");
    this->ui->toolBar->addWidget(buttonSelectDir);
    this->connect(buttonSelectDir,SIGNAL(clicked()),this,SLOT(selectDir()));

    this->ui->toolBar->addSeparator();

    QIcon selectModel(":/icon/icon/daoru.png");

    QPushButton* buttonSelecPlatetModel = new QPushButton(selectModel,QString::fromLocal8Bit("导入车牌识别模型"));
    buttonSelecPlatetModel->setStyleSheet( "QPushButton{background: transparent;}"
                                      "QPushButton:hover{background:gray;}");
    this->connect(buttonSelecPlatetModel,SIGNAL(clicked()),this,SLOT(selectPlateModel()));
    this->ui->toolBar->addWidget(buttonSelecPlatetModel);

    this->ui->toolBar->addSeparator();

    QPushButton* buttonSelectCharModel = new QPushButton(selectModel,QString::fromLocal8Bit("导入字符识别模型"));
    buttonSelectCharModel->setStyleSheet( "QPushButton{background: transparent;}"
                                      "QPushButton:hover{background:gray;}");
    this->connect(buttonSelectCharModel,SIGNAL(clicked()),this,SLOT(selectCharModel()));
    this->ui->toolBar->addWidget(buttonSelectCharModel);

    this->ui->toolBar->addSeparator();

    QIcon recog(":/icon/icon/tableactivation.png");
    QPushButton* buttonRecog = new QPushButton(recog,QString::fromLocal8Bit("识别"));
    buttonRecog->setStyleSheet( "QPushButton{background: transparent;}"
                                      "QPushButton:hover{background:gray;}");
    this->connect(buttonRecog,SIGNAL(clicked()),this,SLOT(recog()));
    this->ui->toolBar->addWidget(buttonRecog);

    this->ui->verticalLayout->setStretchFactor(this->ui->plateWidget,2);
    this->ui->verticalLayout->setStretchFactor(this->ui->charWidget,3);

    buttonRecog->setShortcut(QKeySequence(QLatin1String("Space")));

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
        QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("请先导入车牌识别模型"),
                                 QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    if(this->charModelPath == "")
    {
        QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("请先导入字符识别模型"),
                                 QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    QFileInfo imageInfo(this->currentImagePath);
    QFileInfo plateModel(this->plateModelPath);
    QFileInfo charModel(this->charModelPath);
    if(imageInfo.exists() == false)
    {
        QMessageBox::information(NULL,QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("无效的图像"),
                                 QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    if(plateModel.exists() == false)
    {
        QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("无效的车牌识别模型"),
                                 QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    if(charModel.exists() == false)
    {
        QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("无效的字符识别模型"),
                                 QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    clock_t start = clock();
    cv::Mat image = cv::imread(this->currentImagePath.toLocal8Bit().toStdString());
    auto plateInfos = PlateRecognition_V3::Recognite(image);
    clock_t end = clock();
    int duration = end - start;
    this->ui->plateWidget->clear();
    for(int i=1;i<=plateInfos.size();i++)
    {
        PlateInfo plateInfo = plateInfos[i-1];
        this->generatePlateRegion(plateInfo,i,duration);
        generateProcessRegion(plateInfo,image);
    }
}

void MainWindow::generateProcessRegion(PlateInfo plateInfo, Mat originImage)
{
    this->ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    this->ui->tableWidget->verticalHeader()->setVisible(false);
    this->ui->tableWidget->horizontalHeader()->setVisible(false);

    this->ui->tableWidget->setRowCount(2);
    this->ui->tableWidget->setColumnCount(2);

    QTableWidgetItem* plateItem = new QTableWidgetItem(tr("Plate_Gray"));
    plateItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->ui->tableWidget->setItem(0,0,plateItem);

    cv::Mat matGrayClone = plateInfo.OriginalMat.clone();
    cv::Mat matColorClone = plateInfo.OriginalMat.clone();
    if(plateInfo.OriginalMat.channels()==3)
    {
        cv::cvtColor(matColorClone,matColorClone,cv::COLOR_BGR2RGB);
        cv::cvtColor(matGrayClone,matGrayClone,cv::COLOR_BGR2GRAY);
    }
    QImage plateImage = QImage((const unsigned char*)(matGrayClone.data),
                          matGrayClone.cols,
                          matGrayClone.rows,
                          matGrayClone.cols*matGrayClone.channels(),
                          QImage::Format_Grayscale8);

    plateImage = plateImage.scaled(plateImage.width()*3,plateImage.height()*3,Qt::IgnoreAspectRatio);
    QPixmap platePixmap = QPixmap::fromImage(plateImage);
    QIcon plateIcon(platePixmap);
    this->ui->tableWidget->setItem(1,0,new QTableWidgetItem(plateIcon,tr("")));

    QTableWidgetItem* hogItem = new QTableWidgetItem(tr("Plate_Hog"));
    hogItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->ui->tableWidget->setItem(0,1,hogItem);

    auto hog = PlateCategory_SVM::ComputeHogDescriptors(matColorClone);
    cv::resize(matColorClone,matColorClone,cv::Size(96,32));
    cv::Mat hogMatClone = Utilities::get_hogdescriptor_visu(matColorClone,hog,cv::Size{96,32});
    QImage hogImage = QImage((const unsigned char*)(hogMatClone.data),
                          hogMatClone.cols,
                          hogMatClone.rows,
                          hogMatClone.cols*hogMatClone.channels(),
                          QImage::Format_RGB888);
    hogImage = hogImage.scaled(hogImage.width()*3,hogImage.height()*3,Qt::IgnoreAspectRatio);
    QPixmap hogPixmap = QPixmap::fromImage(hogImage);
    QIcon hogIcon(hogPixmap);
    this->ui->tableWidget->setItem(1,1,new QTableWidgetItem(hogIcon,tr("")));

    this->ui->tableWidget->setIconSize(QSize(300,plateImage.height()*300/plateImage.width()));
    this->ui->tableWidget->setRowHeight(1,185);
    this->ui->tableWidget->setColumnWidth(0,300);
    this->ui->tableWidget->setColumnWidth(1,300);
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
    tableCharInfo->setItem(0,1,new QTableWidgetItem(QString::fromLocal8Bit("字符")));
    tableCharInfo->setItem(0,2,new QTableWidgetItem(QString::fromLocal8Bit("宽度")));
    tableCharInfo->setItem(0,3,new QTableWidgetItem(QString::fromLocal8Bit("高度")));
    tableCharInfo->setItem(0,4,new QTableWidgetItem(QString::fromLocal8Bit("宽高比")));
    tableCharInfo->setItem(0,5,new QTableWidgetItem(QString::fromLocal8Bit("切分方法")));

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
        image = image.scaled(image.width()*2,image.height()*2,Qt::IgnoreAspectRatio);
        QPixmap pixmap = QPixmap::fromImage(image);
        QIcon charIcon(pixmap);
        tableCharInfo->setIconSize(QSize(image.width()*3/2,image.height()*3/2));
        tableCharInfo->setItem(i,0,new QTableWidgetItem(charIcon,tr("")));

        tableCharInfo->setItem(i,1,new QTableWidgetItem(QString::fromLocal8Bit(charInfo.ToString().c_str())));

        tableCharInfo->setItem(i,2,new QTableWidgetItem(QString::number(charInfo.OriginalRect.width)));
        tableCharInfo->setItem(i,3,new QTableWidgetItem(QString::number(charInfo.OriginalRect.height)));

        ostringstream buffer;
        buffer.str("");
        buffer << std::setiosflags(ios::fixed)
               << std::setprecision(2)
               << float(charInfo.OriginalRect.width) / charInfo.OriginalRect.height;
        tableCharInfo->setItem(i,4,new QTableWidgetItem(QString::fromLocal8Bit(buffer.str().c_str())));

        buffer.str("");
        buffer << charInfo.CharSplitMethod;
        tableCharInfo->setItem(i,5,new QTableWidgetItem(QString::fromLocal8Bit(buffer.str().c_str())));
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
    this->ui->charWidget->addTab(tableCharInfo,QString::fromLocal8Bit("字符信息")+QString::number(index));
}
void MainWindow::generatePlateRegion(PlateInfo plateInfo,int index,int duration)
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
    QPixmap pixmap = QPixmap::fromImage(image);
    labelPlateImage->setAlignment(Qt::AlignCenter);
    labelPlateImage->setPixmap(pixmap);
    labelPlateImage->resize(pixmap.size());
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
    tablePlateInfo->setItem(0,0,new QTableWidgetItem(recog,QString::fromLocal8Bit("车牌号")));
    tablePlateInfo->setItem(0,1,new QTableWidgetItem(QString::fromLocal8Bit(plateInfo.ToString().c_str())));

    QIcon cate(":/icon/icon/cate.png");
    tablePlateInfo->setItem(1,0,new QTableWidgetItem(cate,QString::fromLocal8Bit("车牌类别")));
    ostringstream buffer;
    buffer << plateInfo.PlateCategory;
    tablePlateInfo->setItem(1,1,new QTableWidgetItem(QString::fromLocal8Bit(buffer.str().c_str())));

    QIcon iconWidth(":/icon/icon/kuandu.png");
    tablePlateInfo->setItem(2,0,new QTableWidgetItem(iconWidth,QString::fromLocal8Bit("宽度")));
    tablePlateInfo->setItem(2,1,new QTableWidgetItem(QString::number(plateInfo.OriginalRect.width)));

    QIcon iconHeight(":/icon/icon/gaodu.png");
    tablePlateInfo->setItem(3,0,new QTableWidgetItem(iconHeight,QString::fromLocal8Bit("高度")));
    tablePlateInfo->setItem(3,1,new QTableWidgetItem(QString::number(plateInfo.OriginalRect.height)));

    QIcon originalRect(":/icon/icon/rect.png");
    tablePlateInfo->setItem(4,0,new QTableWidgetItem(originalRect,QString::fromLocal8Bit("长宽比")));
    buffer.str("");
    buffer << std::setiosflags(ios::fixed)
           << std::setprecision(2)
           << float(plateInfo.OriginalRect.width) / plateInfo.OriginalRect.height;
    tablePlateInfo->setItem(4,1,new QTableWidgetItem(QString::fromLocal8Bit(buffer.str().c_str())));

    QIcon method(":/icon/icon/method.png");
    tablePlateInfo->setItem(5,0,new QTableWidgetItem(method,QString::fromLocal8Bit("定位方法")));
    buffer.str("");
    buffer << plateInfo.PlateLocateMethod;
    tablePlateInfo->setItem(5,1,new QTableWidgetItem(QString::fromLocal8Bit(buffer.str().c_str())));

    vLayout->addWidget(tablePlateInfo);
    plateRegion->setLayout(vLayout);

    this->ui->plateWidget->addTab(plateRegion,QString::fromLocal8Bit("车牌信息")+QString::number(index));

    this->generateCharRegion(plateInfo,index);

    this->ui->textEdit->append(QString::fromLocal8Bit("识别成功！"));
    this->ui->textEdit->append(QString::fromLocal8Bit("识别耗时: ")+QString::number(duration)+tr("ms."));
}

void MainWindow::selectDir()
{
    this->pathSelected = QFileDialog::getExistingDirectory(this,
                                                           QString::fromLocal8Bit("选择测试图像文件夹"),
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
                this->plateModelPath.isEmpty()?tr("../classifier"):this->plateModelPath,
                "*.yaml");
    qDebug()<<this->plateModelPath;
    if(this->plateModelPath != "")
    {
        PlateCategory_SVM::Load(this->plateModelPath.toLocal8Bit().toStdString());
    }
}

void MainWindow::selectCharModel()
{
    this->charModelPath = QFileDialog::getOpenFileName(
                this,
                tr("Open model"),
                this->charModelPath.isEmpty()?tr("../classifier"):this->charModelPath,
                "*.yaml");
    qDebug()<<this->charModelPath;
    if(this->charModelPath != "")
    {
        PlateChar_SVM::Load(this->charModelPath.toLocal8Bit().toStdString());
    }
}

void MainWindow::imageArea(cv::Mat mat)
{
    this->ui->tabWidget->clear();
    this->ui->textEdit->clear();

    QWidget* tabOriginal = this->generateImageLabel(mat,QImage::Format_RGB888);
    this->ui->tabWidget->addTab(tabOriginal,QString::fromLocal8Bit("测试图像"));

    this->ui->textEdit->clear();
    QFileInfo file(this->currentImagePath);

    QString fileInfo(QString::fromLocal8Bit("路径: ") + this->currentImagePath +
                     QString::fromLocal8Bit("\n大小: ") + QString::number(file.size()) + tr("B") +
                     QString::fromLocal8Bit("\n像素: ") + QString::number(mat.cols) +tr(" * ") + QString::number(mat.rows));

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
