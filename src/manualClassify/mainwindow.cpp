#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//    qDebug()<< "initilization";
//    this->platesGroup = new QButtonGroup(this);
//    this->platesGroup->addButton(ui->plateButton,0);
//    this->platesGroup->addButton(ui->nonplateButton,1);


//    qDebug()<<"add button successful.";

//    connect(ui->plateButton,SIGNAL(clicked()),
//               this,SLOT(slots_clickPlates()));
//    connect(ui->nonplateButton,SIGNAL(clicked()),
//               this,SLOT(slots_clickPlates()));


    //TODO 好像没用到，删了？
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/澳门车牌/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/澳门车牌_两行/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/澳门车牌_内/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/非车牌/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/警牌/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/军牌/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/军牌_两行/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/内集车牌/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/普通车牌/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/普通车牌_两行/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/香港车牌/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/香港车牌_两行/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/香港车牌_内/"));
    plateCategoryPath.append(QDir::currentPath()+tr("/samples/plates/新能源/"));

    charCategoryPath.append(tr("./chars/_0"));
    charCategoryPath.append(tr("./chars/_1"));
    charCategoryPath.append(tr("./chars/_2"));
    charCategoryPath.append(tr("./chars/_3"));
    charCategoryPath.append(tr("./chars/_4"));
    charCategoryPath.append(tr("./chars/_5"));
    charCategoryPath.append(tr("./chars/_6"));
    charCategoryPath.append(tr("./chars/_7"));
    charCategoryPath.append(tr("./chars/_8"));
    charCategoryPath.append(tr("./chars/_9"));
    charCategoryPath.append(tr("./chars/_A"));
    charCategoryPath.append(tr("./chars/_B"));
    charCategoryPath.append(tr("./chars/_C"));
    charCategoryPath.append(tr("./chars/_D"));
    charCategoryPath.append(tr("./chars/_E"));
    charCategoryPath.append(tr("./chars/_F"));
    charCategoryPath.append(tr("./chars/_G"));
    charCategoryPath.append(tr("./chars/_H"));
    charCategoryPath.append(tr("./chars/_I"));
    charCategoryPath.append(tr("./chars/_J"));
    charCategoryPath.append(tr("./chars/_K"));
    charCategoryPath.append(tr("./chars/_L"));
    charCategoryPath.append(tr("./chars/_M"));
    charCategoryPath.append(tr("./chars/_N"));
    charCategoryPath.append(tr("./chars/_O"));
    charCategoryPath.append(tr("./chars/_P"));
    charCategoryPath.append(tr("./chars/_Q"));
    charCategoryPath.append(tr("./chars/_R"));
    charCategoryPath.append(tr("./chars/_S"));
    charCategoryPath.append(tr("./chars/_T"));
    charCategoryPath.append(tr("./chars/_U"));
    charCategoryPath.append(tr("./chars/_V"));
    charCategoryPath.append(tr("./chars/_W"));
    charCategoryPath.append(tr("./chars/_X"));
    charCategoryPath.append(tr("./chars/_Y"));
    charCategoryPath.append(tr("./chars/_Z"));
    charCategoryPath.append(tr("./chars/_澳"));
    charCategoryPath.append(tr("./chars/_藏"));
    charCategoryPath.append(tr("./chars/_川"));
    charCategoryPath.append(tr("./chars/_点"));
    charCategoryPath.append(tr("./chars/_鄂"));
    charCategoryPath.append(tr("./chars/_非字符"));
    charCategoryPath.append(tr("./chars/_甘"));
    charCategoryPath.append(tr("./chars/_赣"));
    charCategoryPath.append(tr("./chars/_港"));
    charCategoryPath.append(tr("./chars/_贵"));
    charCategoryPath.append(tr("./chars/_桂"));
    charCategoryPath.append(tr("./chars/_黑"));
    charCategoryPath.append(tr("./chars/_沪"));
    charCategoryPath.append(tr("./chars/_吉"));
    charCategoryPath.append(tr("./chars/_冀"));
    charCategoryPath.append(tr("./chars/_津"));
    charCategoryPath.append(tr("./chars/_晋"));
    charCategoryPath.append(tr("./chars/_京"));
    charCategoryPath.append(tr("./chars/_警"));
    charCategoryPath.append(tr("./chars/_辽"));
    charCategoryPath.append(tr("./chars/_鲁"));
    charCategoryPath.append(tr("./chars/_蒙"));
    charCategoryPath.append(tr("./chars/_闽"));
    charCategoryPath.append(tr("./chars/_宁"));
    charCategoryPath.append(tr("./chars/_青"));
    charCategoryPath.append(tr("./chars/_琼"));
    charCategoryPath.append(tr("./chars/_陕"));
    charCategoryPath.append(tr("./chars/_苏"));
    charCategoryPath.append(tr("./chars/_穗"));
    charCategoryPath.append(tr("./chars/_皖"));
    charCategoryPath.append(tr("./chars/_湘"));
    charCategoryPath.append(tr("./chars/_新"));
    charCategoryPath.append(tr("./chars/_渝"));
    charCategoryPath.append(tr("./chars/_豫"));
    charCategoryPath.append(tr("./chars/_粤"));
    charCategoryPath.append(tr("./chars/_云"));
    charCategoryPath.append(tr("./chars/_浙"));

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showImagesByPath(QString imagePath)
{
    //QString imagePath=imagePath;
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

void MainWindow::moveFile(QString fullSourceFileName, QString destinationPath)
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
void MainWindow::showImages(int tag)
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

//用标记来制定相应的文件夹
void MainWindow::labelImage(QString imageFilename, int tag)
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

void MainWindow::labelImage(QString imageFilename, QString tag)
{
    if(imageFilename.isEmpty() || imageFilename==NULL)
    {
        qDebug()<< "filename is null";
        return;
    }
    moveFile(imageFilename,plateSamplePath+tag+"/");
}

void MainWindow::updateWindow()
{
    this->ui->imageShowWidget->clear();

    QString comoCurrentName=this->ui->comboBox->currentText();
    /*
    if(comoCurrentName.compare(tr("plates"))==0)
    {
        this->showImages(-1);
        //this->ui->plateButton->setChecked(true);
    }
    else if(comoCurrentName.compare("nonplates")==0)
    {
        this->showImages(-2);
        //this->ui->nonplateButton->setChecked(true);
    }
    else
    {
        qDebug()<<"anything else?";
    }*/

    //qDebug()<<"the path to show："<<plateSamplePath+comoCurrentName+"/";
    showImagesByPath(plateSamplePath+comoCurrentName+"/");

}

//根据Comobox中的信息显示车牌的图片
void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    this->updateWindow();
}

void MainWindow::on_imageShowWidget_itemDoubleClicked(QListWidgetItem *item)
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

void MainWindow::on_actionopenFolder_triggered()
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
