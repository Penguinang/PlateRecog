#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "manualclassifywindow.h"
#include "trainingfrontend.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //加载默认的模型
    PlateCategory_SVM::Load("CategorySVM.yaml");
    PlateChar_SVM::Load("CharSVM.yaml");

    //本窗口关闭时，其他窗口均关闭
    this->setAttribute(Qt::WA_QuitOnClose,true);

    //设置收集样本界面的icon
    this->setWindowIcon(QIcon(":/icons/resourceFiles/plateUI03.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectDir()
{
    this->pathSelected = QFileDialog::getExistingDirectory(this,
                                                           QString::fromLocal8Bit("选择图片所在文件夹"),
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


void MainWindow::showMatForAutoSample(Mat mat)
{
    this->ui->tabWidget->clear();
    QImage image = Mat2QImage(mat,QImage::Format_RGB888);
    //构建Tab页面
    QWidget* tabOriginal = this->generateImageLabel(mat,QImage::Format_RGB888);
    this->ui->tabWidget->addTab(tabOriginal,QString::fromLocal8Bit("原图"));
}

void MainWindow::showMat(cv::Mat mat,vector<tuple<vector<PlateInfo>, Mat, Mat, vector<vector<Point>>, Mat>> plates)
{
    this->ui->tabWidget->clear();
    QImage image = Mat2QImage(mat,QImage::Format_RGB888);
    //构建Tab页面

    //调整显示的图片size，并未改变原图，只在显示时改变size
    if(mat.cols > 1000)
    {
        float rate = 1000/(float)mat.cols;
        int height = mat.rows* rate;

        cv::resize(mat,mat,cv::Size(1000,height));
    }

    /*
    //根据tabwidge调整显示的图片大小，但是当窗口大小改变后会出问题
    int widthTab = this->ui->tabWidget->width();
    if(mat.cols > widthTab)
        {
            float rate = widthTab/(float)mat.cols;
            int height = mat.rows* rate;

            cv::resize(mat,mat,cv::Size(widthTab,height));
        }
    */
    QWidget* tabOriginal = this->generateImageLabel(mat,QImage::Format_RGB888);
    this->ui->tabWidget->addTab(tabOriginal,QString::fromLocal8Bit("原图"));


    Mat thremat = get<1>(plates[0]);
    //调整显示的图片size，并未改变原图，只在显示时改变size
    if(thremat.cols > 1000)
    {
        float rate = 1000/(float)thremat.cols;
        int height = thremat.rows* rate;

        cv::resize(thremat,thremat,cv::Size(1000,height));
    }
    QWidget* tabthreshold = this->generateImageLabel(thremat,QImage::Format_Grayscale8);
    this->ui->tabWidget->addTab(tabthreshold,QString::fromLocal8Bit("二值化"));


    Mat erode = get<2>(plates[0]);
    //调整显示的图片size，并未改变原图，只在显示时改变size
    if(erode.cols > 1000)
    {
        float rate = 1000/(float)erode.cols;
        int height = erode.rows* rate;

        cv::resize(erode,erode,cv::Size(1000,height));
    }
    QWidget* taberod = this->generateImageLabel(erode,QImage::Format_Grayscale8);
    this->ui->tabWidget->addTab(taberod,QString::fromLocal8Bit("形态学操作"));


    Mat rected = get<4>(plates[0]);
    //调整显示的图片size，并未改变原图，只在显示时改变size
    if(rected.cols > 1000)
    {
        float rate = 1000/(float)rected.cols;
        int height = rected.rows* rate;

        cv::resize(rected,rected,cv::Size(1000,height));
    }
    QWidget* tabrect = this->generateImageLabel(rected,QImage::Format_RGB888);
    this->ui->tabWidget->addTab(tabrect,QString::fromLocal8Bit("Rects"));


}


void MainWindow::showPlateSplit(cv::Mat mat, vector<PlateInfo> plateInfos)
{
    this->ui->plateList->clear();

    //布局横向从左到右
    this->ui->plateList->setFlow(QListWidget::LeftToRight);

    if(mat.empty()) return;
    for (int index = 0; index < plateInfos.size(); index++)
    {
        QString category = QString::fromLocal8Bit(PlateCategory_tToString[(int)plateInfos[index].PlateCategory]);
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
        QString category = QString::fromLocal8Bit(PlateChar_tToString[(int)(charInfos[index].PlateChar)]);

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

//对某一张图片图片处理过程，车牌字符分割的显示
void MainWindow::showAllPicturesGotten(QListWidgetItem *item)
{
    if(item == nullptr) return;

    QString imgFileName = item->text();
    imgFileName = this->pathSelected + "/" + imgFileName;
    string std_filename = imgFileName.toLocal8Bit().toStdString();
    cv::Mat mat = cv::imread(std_filename);


    qDebug()<<"locate begin";

    Mat matprocess;//图像处理之后的结果
    //auto plates = PlateLocator_V3::LocatePlatesForAutoSample(mat, matprocess);
    auto plates = PlateLocator_V3::LocatePlatesForAutoSample(mat, matprocess,
                                                             this->ui->blurSizeBox->value(),
                                                             this->ui->sobelScaleBox->value(),
                                                             this->ui->sobelDeltaBox->value(),
                                                             this->ui->sobelXweightBox->value(),
                                                             this->ui->sobelYweightBox->value(),
                                                             this->ui->morphSizeWidthBox->value(),
                                                             this->ui->morphSizeHeightBox->value(),
                                                             this->ui->minWidthBox->value(),
                                                             this->ui->maxWidthBox->value(),
                                                             this->ui->minHeightBox->value(),
                                                             this->ui->maxHeightBox->value(),
                                                             this->ui->minRatioBox->value(),
                                                             this->ui->maxRatioBox->value());
    int sizeOfPlates = plates.size();
    //TODO
    qDebug()<< "num of plateLocator methods used is " + (QString)sizeOfPlates;
    vector<PlateInfo> plateInfos;
    if(sizeOfPlates == 1)
    {
        plateInfos = get<0>(plates[0]);
    }
    else if (sizeOfPlates == 2) {
        plateInfos = get<0>(plates[1]);
    }

    //qDebug()<< "generate platesinfo";
    //qDebug()<< PlateCategory_tToString[(int)plateInfos[0].PlateCategory];



    showMat(mat, plates);

    showPlateSplit(mat, plateInfos);



    //显示字符分割结果
    this->ui->charList->clear();
    for(int index = 0; index < plateInfos.size();index++)
    {
        Mat plateMat = mat(plateInfos[index].OriginalRect);
        auto chars = CharSegment_V3::SplitePlateForAutoSample(plateMat);

        //比较哪个增强结果好
        vector<CharInfo> charInfosByOriginal = get<0>(chars[0]);
        vector<CharInfo> charInfosByIndex = get<0>(chars[1]);
        vector<CharInfo> charInfosByGamma = get<0>(chars[2]);
        vector<CharInfo> charInfosByLog = get<0>(chars[3]);

        int sizeO = charInfosByOriginal.size();
        int sizeI = charInfosByIndex.size();
        int sizeG = charInfosByGamma.size();
        int sizeL = charInfosByLog.size();

        vector<CharInfo> charInfos;

        //比较哪个字符最多
        if((sizeO >= sizeI)&&(sizeO >= sizeG)&&(sizeO >= sizeL))
        {
            charInfos = charInfosByOriginal;
            qDebug() << "charSegement by original";
        }
        else if ((sizeI >= sizeO)&&(sizeI >= sizeG)&&(sizeI >= sizeL)) {
            charInfos = charInfosByIndex;
            qDebug() << "charSegement by index";
        }
        else if ((sizeG >= sizeO)&&(sizeG >= sizeI)&&(sizeG >= sizeL)) {
            charInfos = charInfosByGamma;
            qDebug() << "charSegement by gamma";
        }
        else if ((sizeL >= sizeO)&&(sizeL >= sizeI)&&(sizeL >= sizeG)) {
            charInfos = charInfosByLog;
            qDebug() << "charSegement by log";
        }

 //       charInfos= get<0>(chars[0]);

        showPlateCharSplit(plateMat, charInfos);
    }
}

void MainWindow::on_fileList_itemClicked(QListWidgetItem *item)
{
    showAllPicturesGotten(item);
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
    QString path=this->ui->textEdit->toPlainText();
    QString qsir = path+QString::fromLocal8Bit("/车牌-字符样本/plates/");
    QString localsir = item->text();
    QString fdir = qsir + localsir + "/";

    string dir = fdir.toLocal8Bit().toStdString();
    //string dir = fdir.toStdString();

    saveMatPic(mat, dir);

}

void MainWindow::saveMatPic(Mat mat,string dir)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QString ctime = currentTime.toString("yyyyMMddhhmmsszzz");
    cv::String stime = ctime.toStdString();

    cv::String imgName = dir + stime.append( ".png");
    try {

            bool ret = cv::imwrite(imgName,mat);
            qDebug() << " save success";
            qDebug() << QString::fromStdString(imgName);

    } catch (std::runtime_error & exception)
    {
        fprintf(stderr, "error when saving picture", exception.what());
    }
}



//保存所有车牌
void MainWindow::on_savePlate_clicked()
{
    saveAllPlateLocated();
}

//保存所有字符
void MainWindow::on_saveChar_clicked()
{
    saveAllPlateCharSplited();
}

void MainWindow::on_charList_itemClicked(QListWidgetItem *item)
{
    if(item == nullptr) return;
    int num = this->ui->charList->count();
    QPixmap charPix;
    Mat mat;
    charPix = item->icon().pixmap(item->icon().availableSizes().last());
    mat = QPixmapToMat(charPix);
    QString path=this->ui->textEdit->toPlainText();
    QString qsir = path+QString::fromLocal8Bit("/车牌-字符样本/chars/");
    QString localsir = item->text();
    QString fdir = qsir + localsir + "/";

    string dir = fdir.toLocal8Bit().toStdString();

    saveMatPic(mat, dir);
}



//保存所有定位后疑似车牌的图片
void MainWindow::saveAllPlateLocated()
{
    int num = this->ui->plateList->count();
    QListWidgetItem *item;
    QPixmap platePix;
    Mat mat;
    for(int i = 0; i < num; i++)
    {
        item = ui->plateList->item(i);
        platePix = item->icon().pixmap(item->icon().availableSizes().last());
        mat = QPixmapToMat(platePix);
        QString path=this->ui->textEdit->toPlainText();
        QString qsir = path+QString::fromLocal8Bit("/车牌-字符样本/plates/");
        QString localsir = item->text();
        QString fdir = qsir + localsir + "/";

        string dir = fdir.toLocal8Bit().toStdString();
        saveMatPic(mat, dir);
    }
}

//保存所有分割出来的字符
void MainWindow::saveAllPlateCharSplited()
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

        QString path=this->ui->textEdit->toPlainText();
        QString qsir = path+QString::fromLocal8Bit("/车牌-字符样本/chars/");

        QString localsir = item->text();
        QString fdir = qsir + localsir + "/";

        string dir = fdir.toLocal8Bit().toStdString();

        saveMatPic(mat, dir);
    }
}

void MainWindow::on_openFile_triggered()
{
    selectDir();
}

//自动生成样本
void MainWindow::on_autoCreateSample_triggered()
{
    int num = this->ui->fileList->count();
    QListWidgetItem *item;
    for (int i = 0;i < num; i++)
    {
        item = ui->fileList->item(i);
        if(item == nullptr) return;

        QString imgFileName = item->text();
        imgFileName = this->pathSelected + "/" + imgFileName;

        cv::Mat mat = cv::imread(imgFileName.toStdString());
        //显示图片
        showMatForAutoSample(mat);

        //显示车牌定位结果
        qDebug()<<"locate begin";
        //vector<PlateInfo> plateInfos = PlateLocator_V3::LocatePlates(mat);

        //显示车牌
        vector<PlateInfo> plateInfos = PlateRecognition_V3::Recognite(mat);
        showPlateSplit(mat, plateInfos);
        //显示字符
        this->ui->charList->clear();
        for(int index = 0; index < plateInfos.size();index++)
        {
            Mat plateMat = mat(plateInfos[index].OriginalRect);
            vector<CharInfo> charInfos = plateInfos[index].CharInfos;
            showPlateCharSplit(plateMat, charInfos);
        }


        //保存所有分离出的图片
        saveAllPlateLocated();
        saveAllPlateCharSplited();

    }
}

void MainWindow::on_refresh_clicked()
{
    QListWidgetItem *item = this->ui->fileList->currentItem();
    showAllPicturesGotten(item);
}

void MainWindow::on_checkSample_triggered()
{
    ManualClassifyWindow *mcf = new ManualClassifyWindow ();
    mcf->show();

}

void MainWindow::on_actionTrain_triggered()
{
    trainingFrontEnd *tfe = new trainingFrontEnd();
    tfe->show();
}

void MainWindow::on_pushButton_clicked()
{
    QString pathSelected = QFileDialog::getExistingDirectory(this,
                                                            QString::fromLocal8Bit("选择图片存放路径"),
                                                          QString::fromLocal8Bit("../../bin/"));
    this->ui->textEdit->setText(pathSelected);
}

//关闭窗口时弹出询问框
void MainWindow::closeEvent(QCloseEvent *event)
{
    switch (QMessageBox::information(this,tr("warning!"),
                                     tr("Do you really want to log out this procedure?"),
            tr("Yes"),tr("No"),0,1)) {
    case 0:
        event->accept();
        break;
    case 1:
    default:
        event->ignore();
        break;

    }
}

void MainWindow::on_exit_triggered()
{
    this->close();
}
