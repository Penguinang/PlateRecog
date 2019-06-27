#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QAction* actionSelectDir = new QAction(tr("选择路径"),this);

    this->connect(actionSelectDir,SIGNAL(triggered()),this,SLOT(selectDir()));

    this->ui->mainToolBar->addAction(actionSelectDir);

    this->setWindowState(Qt::WindowState::WindowMaximized);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectDir()
{
    this->pathSelected = QFileDialog::getExistingDirectory(this,
                                                           tr("选择图片所在文件夹"),
                                                           this->pathSelected.isEmpty()?tr("D:/pictures"):this->pathSelected);
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



void MainWindow::showMat(cv::Mat mat)
{
    this->ui->tabWidget->clear();
    QImage image = Mat2QImage(mat,QImage::Format_RGB888);
    //构建Tab页面
    QWidget* tabOriginal = this->generateImageLabel(mat,QImage::Format_RGB888);
    this->ui->tabWidget->addTab(tabOriginal,tr("原图"));

}


void MainWindow::showPlateSplit(cv::Mat mat, vector<PlateInfo> plateInfos)
{
    this->ui->plateList->clear();

    //布局横向从左到右
    this->ui->plateList->setFlow(QListWidget::LeftToRight);

    if(mat.empty()) return;
    for (int index = 0; index < plateInfos.size(); index++)
    {
        QString category = PlateCategory_tToString[(int)plateInfos[index].PlateCategory];
        cv::Mat roi = mat(plateInfos[index].OriginalRect);

        QPixmap pixmap = QPixmap::fromImage(Mat2QImage(roi,QImage::Format_RGB888));

        QIcon iconImage(pixmap);

        QListWidgetItem *listWidgetItem = new QListWidgetItem();

        //设置icon大小
        ui->plateList->setViewMode(QListView::IconMode);
        ui->plateList->setIconSize(QSize(300,300));

        ui->plateList->setSpacing(10);
        ui->plateList->setResizeMode(QListWidget::Adjust);

        listWidgetItem->setIcon(iconImage);
        listWidgetItem->setText(category);

        this->ui->plateList->addItem(listWidgetItem);
    }

}

void MainWindow::showPlateCharSplit(cv::Mat plateMat, vector<CharInfo> charInfos)
{
    //this->ui->charList->clear();

    if(plateMat.empty()) return;
    for (int index = 0; index < charInfos.size(); index++)
    {
        //qDebug()<<charInfos[index].ToString();
        QString category = PlateChar_tToString[(int)(charInfos[index].PlateChar)];

        cv::Mat roi = plateMat(charInfos[index].OriginalRect);
        QPixmap pixmap = QPixmap::fromImage(Mat2QImage(roi,QImage::Format_RGB888));

        QIcon iconImage(pixmap);
        QListWidgetItem *listWidgetItem = new QListWidgetItem();

        //设置icon大小
        ui->charList->setViewMode(QListView::IconMode);
        ui->charList->setIconSize(QSize(300,300));
        ui->charList->setSpacing(10);
        ui->charList->setResizeMode(QListWidget::Adjust);

        listWidgetItem->setIcon(iconImage);
        listWidgetItem->setText(category);
        this->ui->charList->addItem(listWidgetItem);
    }

}

QImage MainWindow::Mat2QImage(cv::Mat mat, QImage::Format format)
{
    cv::Mat temp = mat.clone();

    if(mat.channels() == 3) cv::cvtColor(temp,mat,cv::COLOR_BGR2RGB);

    QImage image = QImage((const unsigned char*)(temp.data),
                                  temp.cols,
                                  temp.rows,
                                  temp.cols * temp.channels(),
                                  format).copy();

     temp.release();

     return image;

}

Mat MainWindow::QImageToMat(QImage image)
{
    QImage swapped = image.copy();
    if(image.format() == QImage::Format_RGB32)
    {
        swapped = swapped.convertToFormat(QImage::Format_RGB888);
    }

    swapped = swapped.rgbSwapped();
    return Mat(swapped.height(),swapped.width(),CV_8UC3,
               const_cast<uchar*>(swapped.bits()),
               static_cast<size_t>(swapped.bytesPerLine())
               ).clone();
}

Mat MainWindow::QPixmapToMat(QPixmap pixmap)
{
    return QImageToMat(pixmap.toImage());
}

QLabel* MainWindow::generateImageLabel(cv::Mat mat,QImage::Format format)
{
    QLabel* lblImage = new QLabel();
    QImage image = Mat2QImage(mat,format);
    QPixmap pixmap = QPixmap::fromImage(image);
    lblImage->setPixmap(pixmap);
    lblImage->resize(pixmap.size());

    return lblImage;
}

void MainWindow::on_fileList_itemClicked(QListWidgetItem *item)
{

    if(item == nullptr) return;

    QString imgFileName = item->text();
    imgFileName = this->pathSelected + "/" + imgFileName;

    cv::Mat mat = cv::imread(imgFileName.toStdString());
    showMat(mat);

    qDebug()<<"locate begin";
    //vector<PlateInfo> plateInfos = PlateLocator_V3::LocatePlates(mat);
    Mat matProcess; //记录处理过的矩阵
    //调用车牌定位函数时注释掉了platlocator_v3里面函数里的svm给车牌分类部分
    PlateCategory_SVM::Load("CategorySVM.yaml");
    vector<PlateInfo> plateInfos = PlateLocator_V3::LocatePlatesForAutoSample(mat,matProcess);
    qDebug()<< "generate platesinfo";
    qDebug()<< PlateCategory_tToString[(int)plateInfos[0].PlateCategory];
    showPlateSplit(mat, plateInfos);

    //显示字符分割结果
    this->ui->charList->clear();
    for(int index = 0; index < plateInfos.size();index++)
    {
        Mat plateMat = mat(plateInfos[index].OriginalRect);
        //注释了charsegment_V3函数里SVM分类
        PlateChar_SVM::Load("CharSVM.yaml");
        vector<CharInfo> charInfos = CharSegment_V3::SplitePlateForAutoSample(plateMat);
        showPlateCharSplit(plateMat, charInfos);
    }



}

//当车牌list里item被点击
void MainWindow::on_plateList_itemClicked(QListWidgetItem *item)
{
    if(item == nullptr) return;
    int num = this->ui->plateList->count();
    QPixmap platePix;
    Mat mat;
    platePix = item->icon().pixmap(item->icon().availableSizes().last());
    mat = QPixmapToMat(platePix);
    QString qsir = "F:\\test\\车牌-字符样本\\plates\\";
    QString localsir = item->text();
    QString fdir = qsir + localsir + "\\";

    string dir = fdir.toLocal8Bit().toStdString();

    saveMatPic(mat, dir);

}

void MainWindow::saveMatPic(Mat mat,string dir)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QString ctime = currentTime.toString("yyyyMMddhhmmsszzz");
    cv::String stime = ctime.toStdString();

    //想要存放的文件夹路径
    //cv::String dir = "E:\\test\\pictures_split\\";
    cv::String imgName = dir + stime.append( ".png");
    try {

            cv::imwrite(imgName,mat);
            //qDebug() << " save success";
            //qDebug() << QString::fromStdString(imgName);

    } catch (std::runtime_error & exception)
    {
        fprintf(stderr, "error when saving picture", exception.what());
    }
}




void MainWindow::on_savePlate_clicked()
{
    //保存所有图片
    int num = this->ui->plateList->count();
    QListWidgetItem *item;
    QPixmap platePix;
    Mat mat;
    for(int i = 0; i < num; i++)
    {
        //item = ui->plateList->takeItem(i);
        item = ui->plateList->item(i);
        platePix = item->icon().pixmap(item->icon().availableSizes().last());
        mat = QPixmapToMat(platePix);

        QString qsir = "F:\\test\\车牌-字符样本\\plates\\";
        QString localsir = item->text();
        QString fdir = qsir + localsir + "\\";

        string dir = fdir.toLocal8Bit().toStdString();
        saveMatPic(mat, dir);
    }
}

void MainWindow::on_saveChar_clicked()
{
    int num = this->ui->charList->count();
    QListWidgetItem *item;
    QPixmap charPix;
    Mat mat;
    for(int i = 0; i < num; i++)
    {
        item = ui->charList->item(i);
        charPix = item->icon().pixmap(item->icon().availableSizes().last());
        mat = QPixmapToMat(charPix);

        QString qsir = "F:\\test\\车牌-字符样本\\chars\\";
        QString localsir = item->text();
        QString fdir = qsir + localsir + "\\";

        string dir = fdir.toLocal8Bit().toStdString();

        saveMatPic(mat, dir);
    }
}

void MainWindow::on_charList_itemClicked(QListWidgetItem *item)
{
    if(item == nullptr) return;
    int num = this->ui->charList->count();
    QPixmap charPix;
    Mat mat;
    charPix = item->icon().pixmap(item->icon().availableSizes().last());
    mat = QPixmapToMat(charPix);

    QString qsir = "F:\\test\\车牌-字符样本\\chars\\";
    QString localsir = item->text();
    QString fdir = qsir + localsir + "\\";

    string dir = fdir.toLocal8Bit().toStdString();

    saveMatPic(mat, dir);
}
