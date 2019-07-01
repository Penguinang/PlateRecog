#include "manualclassifywindow.h"
#include "ui_manualclassifywindow.h"



ManualClassifyWindow::ManualClassifyWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::ManualClassifyWindow)
{
    this->basePath = QFileDialog::getExistingDirectory(this,
                                                           ("Please choose base sample file folder"),
                                                            "D:/Github/PlateRecog/bin");
    this->plateSamplePath=this->basePath+"/plates/";
    this->charSamplepath=this->basePath+"/chars/";

    //主窗口关闭时，本窗口也关闭
    this->setAttribute(Qt::WA_QuitOnClose,false);

    ui->setupUi(this);
}


ManualClassifyWindow::~ManualClassifyWindow()
{
    delete ui;
}

void ManualClassifyWindow::showImagesByPath(QString imagePath)
{
    QDir* dir = new QDir(imagePath);
    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.png" << "*.bmp";
    //将文件名称添加到List中
    QStringList imgFileNames = dir->entryList(nameFilters);

    for(QString imgFilename:imgFileNames)
    {
        //在显示区域显示出来
        this->ui->imageShowWidget->setViewMode(QListWidget::IconMode);

        this->ui->imageShowWidget->setIconSize(QSize(200,200));
        this->ui->imageShowWidget->setFlow(QListView::LeftToRight);
        this->ui->imageShowWidget->setTextElideMode(Qt::ElideMiddle);
        //this->ui->imageShowWidget->setResizeMode(QListWidget::Adjust);

        //读取图片
        QPixmap pixmap ;
        QString fullImagePath=imagePath+imgFilename;//文件全路径
        //重新设置以下大小
        pixmap.load(fullImagePath);
        pixmap = pixmap.scaled(100,40);
        //定义QListWidgetItem对象
        QListWidgetItem *imageItem = new QListWidgetItem(QIcon(pixmap),"");
        imageItem->setWhatsThis(fullImagePath);

        //QListWidgetItem *imageItem = new QListWidgetItem;
        //imageItem->setIcon(pixmap);

        this->ui->imageShowWidget->addItem(imageItem);
        //qDebug()<< "fullImagePath:" << fullImagePath;
    }
}

void ManualClassifyWindow::showCharImageByPath(QString charImagePath)
{
    QDir* dir = new QDir(charImagePath);
    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.png" << "*.bmp";
    //将文件名称添加到List中
    QStringList imgFileNames = dir->entryList(nameFilters);

    for(QString imgFilename:imgFileNames)
    {
        //在显示字符图像的区域显示出来
        this->ui->charListWidget->setViewMode(QListWidget::IconMode);

        this->ui->charListWidget->setIconSize(QSize(200,200));
        this->ui->charListWidget->setFlow(QListView::LeftToRight);
        this->ui->charListWidget->setTextElideMode(Qt::ElideMiddle);

        //读取图片
        QPixmap pixmap ;
        QString fullImagePath=charImagePath+imgFilename;//文件全路径
        //重新设置图片大小
        pixmap.load(fullImagePath);
        pixmap = pixmap.scaled(15,40);
        //定义QListWidgetItem对象
        QListWidgetItem *imageItem = new QListWidgetItem(QIcon(pixmap),"");
        imageItem->setWhatsThis(fullImagePath);

        this->ui->charListWidget->addItem(imageItem);
        //qDebug()<< "fullImagePath:" << fullImagePath;
    }
}

void ManualClassifyWindow::moveFile(QString fullSourceFileName, QString destinationPath)
{
    QFileInfo fileinfo(fullSourceFileName);//原文件
    QFile file(fullSourceFileName);
    qDebug()<<"imageFilename: "+fullSourceFileName ;
    if(!QFile::exists(destinationPath+fileinfo.fileName()))
    {
        qDebug()<<"destinationPath: "+destinationPath+fileinfo.fileName() ;
        if(QFile::copy(fullSourceFileName, destinationPath+fileinfo.fileName()))
        {
            qDebug()<<"copy successful";
            //删除原文件
            file.remove();
        }
        else
        {
            qDebug()<<"copy fail";
        }
    }
    else
    {
        file.remove();
        qDebug()<<"file already exists";
    }
}

//显示相关分类文件夹下的图片
void ManualClassifyWindow::showImages(int tag)
{
    //TODO： 相应文件类的地址
    switch (tag) {
    case 0 :
        showImagesByPath(plateCategoryPath.at(0));
    break;
    case 1 :
        showImagesByPath(plateCategoryPath.at(1));
        break;
     case 2:
        break;
        //TODO 加入相应的地址
    default:
        qDebug() << "no such value";
    }

    /*
    if(tag==1)
    {
        //打开相应文件所在路径
        QString imagePath="D:/picture/test/p/";//先写死
        QDir* dir = new QDir(imagePath);
        QStringList nameFilters;
        nameFilters << "*.jpg" << "*.png" << "*.bmp";
        //将文件名称添加到List中
        QStringList imgFileNames = dir->entryList(nameFilters);

        for(QString imgFilename:imgFileNames)
        {
            //在显示区域显示出来
            this->ui->imageShowWidget->setViewMode(QListWidget::IconMode);
            this->ui->imageShowWidget->setIconSize(QSize(400,400));
            //this->ui->imageShowWidget->setFlow(QListView::LeftToRight);

            //读取图片
            QPixmap pixmap ;
            QString fullImagePath=imagePath+imgFilename;//文件全路径
            //重新设置以下大小
            pixmap.load(fullImagePath);
            pixmap.scaled(30,30);
            //定义QListWidgetItem对象
            QListWidgetItem *imageItem = new QListWidgetItem(QIcon(pixmap),imgFilename);
            //QListWidgetItem *imageItem = new QListWidgetItem;
            //imageItem->setIcon(pixmap);

            this->ui->imageShowWidget->addItem(imageItem);
            //qDebug()<< "fullImagePath:" << fullImagePath;
        }

    }
    else
    {
        //打开相应文件所在路径
        QString imagePath="D:/picture/test/n/";//先写死
        QDir* dir = new QDir(imagePath);
        QStringList nameFilters;
        nameFilters << "*.jpg" << "*.png" << "*.bmp";
        //将文件名称添加到List中
        QStringList imgFileNames = dir->entryList(nameFilters);

        for(QString imgFilename:imgFileNames)
        {
            //在显示区域显示出来
            this->ui->imageShowWidget->setViewMode(QListWidget::IconMode);
            this->ui->imageShowWidget->setIconSize(QSize(400,400));
            this->ui->imageShowWidget->setFlow(QListView::LeftToRight);

            //读取图片
            QPixmap pixmap ;
            QString fullImagePath=imagePath+imgFilename;//文件全路径
            pixmap.load(fullImagePath);
            pixmap.scaled(30,30);

            //定义QListWidgetItem对象
            QListWidgetItem *imageItem = new QListWidgetItem(QIcon(pixmap),imgFilename);
            //QListWidgetItem *imageItem= new QListWidgetItem;
            //imageItem->setIcon(pixmap);

            this->ui->imageShowWidget->addItem(imageItem);
            qDebug()<< "fullImagePath:" << fullImagePath;
        }
    }
    */
}

//你删了这个
//用标记来制定相应的文件夹
void ManualClassifyWindow::labelImage(QString imageFilename, int tag)
{
    if(imageFilename.isEmpty() || imageFilename==NULL)
    {
        qDebug()<< "filename is null";
        return;
    }

    //TODO  目的文件地址
    switch (tag) {
    case -1 :
        moveFile(imageFilename,"D:/picture/test/p/");
    break;
    case -2 :
        moveFile(imageFilename,"D:/picture/test/n/");
        break;
    case 0:
        //TODO 加入相关的路径
        break;
    default:
        qDebug() << "no such value";
    }

    /*
    //将文件保存到相应文件夹
    if(tag==1)
    {
        QString pfolderPath="D:/picture/test/p/";//要存到的文件夹路径，先写死
        QFileInfo fileinfo(imageFilename);//原文件
        QFile file(imageFilename);
        qDebug()<<"imageFilename: "+imageFilename +" filename: "+fileinfo.fileName();
        if(!QFile::exists(pfolderPath+fileinfo.fileName()))
        {
            if(QFile::copy(imageFilename, pfolderPath+fileinfo.fileName()))
            {
                qDebug()<<"copy successful";
                //删除原文件
                file.remove();
            }
        }
        else
        {
            qDebug()<<"file already exists";
        }
    }
    else
    {
        QString pfolderPath="D:/picture/test/n/";//要存到的文件夹路径，先写死
        QFileInfo fileinfo(imageFilename);//原文件
        QFile file(imageFilename);
        if(!QFile::exists(pfolderPath+fileinfo.fileName()))
        {
            if(QFile::copy(imageFilename, pfolderPath+fileinfo.fileName()))
            {
                qDebug()<<"copy successful";
                //删除原文件
                file.remove();
            }
        }
        else
        {
            qDebug()<<"file already exists";
        }
    }
    */
}

void ManualClassifyWindow::labelImage(QString imageFilename, QString tag)
{
    if(imageFilename.isEmpty() || imageFilename==NULL)
    {
        qDebug()<< "filename is null";
        return;
    }
    moveFile(imageFilename,plateSamplePath+tag+"/");
}

void ManualClassifyWindow::labelCharImage(QString imageFilename, QString tag)
{
    if(imageFilename.isEmpty() || imageFilename==NULL)
    {
        qDebug()<< "filename is null";
        return;
    }
    moveFile(imageFilename,this->charSamplepath+tag+"/");
}

void ManualClassifyWindow::updateWindow()
{
    this->ui->imageShowWidget->clear();

    QString comoCurrentName=this->ui->comboBox->currentText();

    //qDebug()<<"the path to show："<<plateSamplePath+comoCurrentName+"/";
    showImagesByPath(plateSamplePath+comoCurrentName+"/");

}

//更新显示数字图片的窗口
void ManualClassifyWindow::updateNumberWindow()
{
    this->ui->charListWidget->clear();
    QString currentNumberName=this->ui->comboBox_number->currentText();
    showCharImageByPath(charSamplepath+currentNumberName+"/");
}

//更新显示字母图片的窗口
void ManualClassifyWindow::updateLetterWindow()
{
    this->ui->charListWidget->clear();
    QString currentLetterName=this->ui->comboBox_char->currentText();
    showCharImageByPath(charSamplepath+currentLetterName+"/");
}

//更新显示汉字的窗口
void ManualClassifyWindow::updateHanziWindow()
{
    this->ui->charListWidget->clear();
    QString currentHanziName=this->ui->comboBox_chineseChar->currentText();
    showCharImageByPath(charSamplepath+currentHanziName+"/");
}

void ManualClassifyWindow::updateCharWindow()
{
    //更新
    qDebug()<<this->ui->tabWidget_allchars->currentIndex();
    switch (this->ui->tabWidget_allchars->currentIndex()) {
    case 0:
        this->updateNumberWindow();
        break;
    case 1:
        this->updateLetterWindow();
        break;
    case 2:
        this->updateHanziWindow();
        break;
    default:
        qDebug()<<"didn't choose a tab";
    }

}

//根据Comobox中的信息显示车牌的图片
void ManualClassifyWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    this->updateWindow();
}

void ManualClassifyWindow::on_imageShowWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString imgName;
    imgName=item->whatsThis();
    //qDebug()<<"current imgName is: "+imgName;

    if(this->ui->aomenplateButton->isChecked())
    {
        //澳门车牌
       //qDebug()<< "I konw plateButton is ture.";
        //this->labelImage(imgName,1);
        this->labelImage(imgName,this->ui->aomenplateButton->text());
    }
    else if(this->ui->anmeoTwoplateButton->isChecked())
    {
        //澳门车牌两行
        //this->labelImage(imgName,-1);
        this->labelImage(imgName,this->ui->aomenInsideButton->text());
    }
    else if(this->ui->aomenInsideButton->isChecked())
    {
        //澳门车牌内
        this->labelImage(imgName,this->ui->aomenInsideButton->text());
    }
    else if(this->ui->nonPlateButton->isChecked())
    {
        //非车牌
        this->labelImage(imgName,this->ui->nonPlateButton->text());
    }
    else if(this->ui->jinPlateButton->isChecked())
    {
        //警牌
        this->labelImage(imgName,this->ui->jinPlateButton->text());
    }
    else if(this->ui->junPlateButton->isChecked())
    {
       //军牌
        this->labelImage(imgName,this->ui->junPlateButton->text());
    }
    else if(this->ui->junTwoButton->isChecked())
    {
      //军牌两行
        this->labelImage(imgName,this->ui->junTwoButton->text());
    }
    else if(this->ui->insidePlateButton->isChecked())
    {
       //内集车牌
        this->labelImage(imgName,this->ui->insidePlateButton->text());
    }
    else if(this->ui->normalplateButton->isChecked())
    {
       //普通车牌
        this->labelImage(imgName,this->ui->normalplateButton->text());
    }
    else if(this->ui->normalTwoButton->isChecked())
    {
       //普通车牌两行
        this->labelImage(imgName,this->ui->normalTwoButton->text());
    }
    else if(this->ui->xiangganButton->isChecked())
    {
       //香港车牌
        this->labelImage(imgName,this->ui->xiangganButton->text());
    }
    else if(this->ui->xianggangTwoButton->isChecked())
    {
       //香港车牌两行
        this->labelImage(imgName,this->ui->xianggangTwoButton->text());
    }
    else if(this->ui->xianggangInsideButton->isChecked())
    {
       //香港车牌内
        this->labelImage(imgName,this->ui->xianggangInsideButton->text());
    }
    else if(this->ui->newEnergyPlateButton->isChecked())
    {
       //新能源
        this->labelImage(imgName,this->ui->newEnergyPlateButton->text());
    }
    else
    {
        qDebug()<<"no tag";
    }

    this->updateWindow();
}

void ManualClassifyWindow::on_actionopenFolder_triggered()
{
    QString pathSelected = QFileDialog::getExistingDirectory(this,                                                           ("choose file folder"),
                                                            "D:/picture/test/");
    if(pathSelected.isEmpty())
    {
        qDebug()<<"you didn't choose a folder";
        return;
    }

    showImagesByPath(pathSelected+"/");
}

void ManualClassifyWindow::on_comboBox_number_currentTextChanged(const QString &arg1)
{
    this->updateCharWindow();
}

void ManualClassifyWindow::on_comboBox_char_currentTextChanged(const QString &arg1)
{
    this->updateCharWindow();
}

void ManualClassifyWindow::on_comboBox_chineseChar_currentTextChanged(const QString &arg1)
{
    this->updateCharWindow();
}

void ManualClassifyWindow::on_charListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString imgName;
    imgName=item->whatsThis();

    if(this->ui->radioButton_0->isChecked())
    {
        //字符_0
        this->labelCharImage(imgName,this->ui->radioButton_0->text());
    }
    else if(this->ui->radioButton_1->isChecked())
    {
        //字符_1
        this->labelCharImage(imgName,this->ui->radioButton_1->text());
    }
    else if(this->ui->radioButton_2->isChecked())
    {
        //字符_2
        this->labelCharImage(imgName,this->ui->radioButton_2->text());
    }
    else if(this->ui->radioButton_3->isChecked())
    {
        //字符_3
        this->labelCharImage(imgName,this->ui->radioButton_3->text());
    }
    else if(this->ui->radioButton_4->isChecked())
    {
        //字符_4
        this->labelCharImage(imgName,this->ui->radioButton_4->text());
    }
    else if(this->ui->radioButton_5->isChecked())
    {
       //字符_5
        this->labelCharImage(imgName,this->ui->radioButton_5->text());
    }
    else if(this->ui->radioButton_6->isChecked())
    {
      //字符_6
        this->labelCharImage(imgName,this->ui->radioButton_6->text());
    }
    else if(this->ui->radioButton_7->isChecked())
    {
       //字符_7
        this->labelCharImage(imgName,this->ui->radioButton_7->text());
    }
    else if(this->ui->radioButton_8->isChecked())
    {
       //字符_8
        this->labelCharImage(imgName,this->ui->radioButton_8->text());
    }
    else if(this->ui->radioButton_9->isChecked())
    {
       //字符_9
        this->labelCharImage(imgName,this->ui->radioButton_9->text());
    }
    else if(this->ui->radioButton_A->isChecked())
    {
       //字符A
        this->labelCharImage(imgName,this->ui->radioButton_A->text());
    }
    else if(this->ui->radioButton_B->isChecked())
    {
       //字符B
        this->labelCharImage(imgName,this->ui->radioButton_B->text());
    }
    else if(this->ui->radioButton_C->isChecked())
    {
       //字符C
        this->labelCharImage(imgName,this->ui->radioButton_C->text());
    }
    else if(this->ui->radioButton_D->isChecked())
    {
       //字符D
        this->labelCharImage(imgName,this->ui->radioButton_D->text());
    }
    else if(this->ui->radioButton_E->isChecked())
    {
       //字符E
        this->labelCharImage(imgName,this->ui->radioButton_E->text());
    }
    else if(this->ui->radioButton_F->isChecked())
    {
       //字符F
        this->labelCharImage(imgName,this->ui->radioButton_F->text());
    }
    else if(this->ui->radioButton_G->isChecked())
    {
       //字符G
        this->labelCharImage(imgName,this->ui->radioButton_G->text());
    }
    else if(this->ui->radioButton_H->isChecked())
    {
       //字符H
        this->labelCharImage(imgName,this->ui->radioButton_H->text());
    }
    else if(this->ui->radioButton_I->isChecked())
    {
       //字符I
        this->labelCharImage(imgName,this->ui->radioButton_I->text());
    }
    else if(this->ui->radioButton_J->isChecked())
    {
       //字符J
        this->labelCharImage(imgName,this->ui->radioButton_J->text());
    }
    else if(this->ui->radioButton_K->isChecked())
    {
       //字符K
        this->labelCharImage(imgName,this->ui->radioButton_K->text());
    }
    else if(this->ui->radioButton_L->isChecked())
    {
       //字符L
        this->labelCharImage(imgName,this->ui->radioButton_L->text());
    }
    else if(this->ui->radioButton_M->isChecked())
    {
       //字符M
        this->labelCharImage(imgName,this->ui->radioButton_M->text());
    }
    else if(this->ui->radioButton_N->isChecked())
    {
       //字符N
        this->labelCharImage(imgName,this->ui->radioButton_N->text());
    }
    else if(this->ui->radioButton_O->isChecked())
    {
       //字符O
        this->labelCharImage(imgName,this->ui->radioButton_O->text());
    }
    else if(this->ui->radioButton_P->isChecked())
    {
       //字符P
        this->labelCharImage(imgName,this->ui->radioButton_P->text());
    }
    else if(this->ui->radioButton_Q->isChecked())
    {
       //字符Q
        this->labelCharImage(imgName,this->ui->radioButton_Q->text());
    }
    else if(this->ui->radioButton_R->isChecked())
    {
       //字符R
        this->labelCharImage(imgName,this->ui->radioButton_R->text());
    }
    else if(this->ui->radioButton_S->isChecked())
    {
       //字符S
        this->labelCharImage(imgName,this->ui->radioButton_S->text());
    }
    else if(this->ui->radioButton_T->isChecked())
    {
       //字符T
        this->labelCharImage(imgName,this->ui->radioButton_T->text());
    }
    else if(this->ui->radioButton_U->isChecked())
    {
       //字符U
        this->labelCharImage(imgName,this->ui->radioButton_U->text());
    }
    else if(this->ui->radioButton_V->isChecked())
    {
       //字符V
        this->labelCharImage(imgName,this->ui->radioButton_V->text());
    }
    else if(this->ui->radioButton_W->isChecked())
    {
       //字符W
        this->labelCharImage(imgName,this->ui->radioButton_W->text());
    }
    else if(this->ui->radioButton_X->isChecked())
    {
       //字符X
        this->labelCharImage(imgName,this->ui->radioButton_X->text());
    }
    else if(this->ui->radioButton_Y->isChecked())
    {
       //字符Y
        this->labelCharImage(imgName,this->ui->radioButton_Y->text());
    }
    else if(this->ui->radioButton_Z->isChecked())
    {
       //字符Z
        this->labelCharImage(imgName,this->ui->radioButton_Z->text());
    }
    else if(this->ui->radioButton_ao->isChecked())
    {
       //汉字澳
        this->labelCharImage(imgName,this->ui->radioButton_ao->text());
    }
    else if(this->ui->radioButton_zang->isChecked())
    {
       //汉字藏
        this->labelCharImage(imgName,this->ui->radioButton_zang->text());
    }
    else if(this->ui->radioButton_chuan->isChecked())
    {
       //汉字川
        this->labelCharImage(imgName,this->ui->radioButton_chuan->text());
    }
    else if(this->ui->radioButton_dian->isChecked())
    {
       //点
        this->labelCharImage(imgName,this->ui->radioButton_dian->text());
    }
    else if(this->ui->radioButton_e->isChecked())
    {
       //汉字鄂
        this->labelCharImage(imgName,this->ui->radioButton_e->text());
    }
    else if(this->ui->radioButton_nonchar->isChecked())
    {
       //非字符
        this->labelCharImage(imgName,this->ui->radioButton_nonchar->text());
    }
    else if(this->ui->radioButton_gan1->isChecked())
    {
       //甘
        this->labelCharImage(imgName,this->ui->radioButton_gan1->text());
    }
    else if(this->ui->radioButton_gan4->isChecked())
    {
       //赣
        this->labelCharImage(imgName,this->ui->radioButton_gan4->text());
    }
    else if(this->ui->radioButton_gang->isChecked())
    {
       //港
        this->labelCharImage(imgName,this->ui->radioButton_gang->text());
    }
    else if(this->ui->radioButton_gui->isChecked())
    {
       //贵
        this->labelCharImage(imgName,this->ui->radioButton_gui->text());
    }
    else if(this->ui->radioButton_mugui->isChecked())
    {
       //桂
        this->labelCharImage(imgName,this->ui->radioButton_mugui->text());
    }
    else if(this->ui->radioButton_hei->isChecked())
    {
       //黑
        this->labelCharImage(imgName,this->ui->radioButton_hei->text());
    }
    else if(this->ui->radioButton_hu->isChecked())
    {
       //沪
        this->labelCharImage(imgName,this->ui->radioButton_hu->text());
    }
    else if(this->ui->radioButton_ji->isChecked())
    {
       //吉
        this->labelCharImage(imgName,this->ui->radioButton_ji->text());
    }
    else if(this->ui->radioButton_ji4->isChecked())
    {
       //冀
        this->labelCharImage(imgName,this->ui->radioButton_ji4->text());
    }
    else if(this->ui->radioButton_jing1->isChecked())
    {
       //津
        this->labelCharImage(imgName,this->ui->radioButton_jing1->text());
    }
    else if(this->ui->radioButton_jin4->isChecked())
    {
       //晋
        this->labelCharImage(imgName,this->ui->radioButton_jin4->text());
    }
    else if(this->ui->radioButton_jing1->isChecked())
    {
       //京
        this->labelCharImage(imgName,this->ui->radioButton_jing1->text());
    }
    else if(this->ui->radioButton_jing3->isChecked())
    {
       //警
        this->labelCharImage(imgName,this->ui->radioButton_jing3->text());
    }
    else if(this->ui->radioButton_liao->isChecked())
    {
       //辽
        this->labelCharImage(imgName,this->ui->radioButton_liao->text());
    }
    else if(this->ui->radioButton_lu->isChecked())
    {
       //鲁
        this->labelCharImage(imgName,this->ui->radioButton_lu->text());
    }
    else if(this->ui->radioButton_meng->isChecked())
    {
       //蒙
        this->labelCharImage(imgName,this->ui->radioButton_meng->text());
    }
    else if(this->ui->radioButton_min->isChecked())
    {
       //闽
        this->labelCharImage(imgName,this->ui->radioButton_min->text());
    }
    else if(this->ui->radioButton_ning->isChecked())
    {
       //宁
        this->labelCharImage(imgName,this->ui->radioButton_ning->text());
    }
    else if(this->ui->radioButton_qing->isChecked())
    {
       //青
        this->labelCharImage(imgName,this->ui->radioButton_qing->text());
    }
    else if(this->ui->radioButton_qiong->isChecked())
    {
       //琼
        this->labelCharImage(imgName,this->ui->radioButton_qiong->text());
    }
    else if(this->ui->radioButton_shan->isChecked())
    {
       //陕
        this->labelCharImage(imgName,this->ui->radioButton_shan->text());
    }
    else if(this->ui->radioButton_su->isChecked())
    {
       //苏
        this->labelCharImage(imgName,this->ui->radioButton_su->text());
    }
    else if(this->ui->radioButton_sui->isChecked())
    {
       //穗
        this->labelCharImage(imgName,this->ui->radioButton_sui->text());
    }
    else if(this->ui->radioButton_wan->isChecked())
    {
       //皖
        this->labelCharImage(imgName,this->ui->radioButton_wan->text());
    }
    else if(this->ui->radioButton_xiang->isChecked())
    {
       //湘
        this->labelCharImage(imgName,this->ui->radioButton_xiang->text());
    }
    else if(this->ui->radioButton_xin->isChecked())
    {
       //新
        this->labelCharImage(imgName,this->ui->radioButton_xin->text());
    }
    else if(this->ui->radioButton_yu->isChecked())
    {
       //渝
        this->labelCharImage(imgName,this->ui->radioButton_yu->text());
    }
    else if(this->ui->radioButton_yu->isChecked())
    {
       //豫
        this->labelCharImage(imgName,this->ui->radioButton_yu->text());
    }
    else if(this->ui->radioButton_yue->isChecked())
    {
       //粤
        this->labelCharImage(imgName,this->ui->radioButton_yue->text());
    }
    else if(this->ui->radioButton_yun->isChecked())
    {
       //云
        this->labelCharImage(imgName,this->ui->radioButton_yun->text());
    }
    else if(this->ui->radioButton_zhe->isChecked())
    {
       //浙
        this->labelCharImage(imgName,this->ui->radioButton_zhe->text());
    }
    else
    {
        qDebug()<<"no tag";
    }

    this->updateCharWindow();
}

void ManualClassifyWindow::closeEvent(QCloseEvent *event)
{
    switch (QMessageBox::information(this,tr("warning!"),
                                     tr("Do you really want to close the checking Window?"),
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
