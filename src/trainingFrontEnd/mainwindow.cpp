// qt headers
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QIcon>
#include <QImage>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QMetaType>
#include <QPixmap>
#include <QThread>
#include <QTime>

// standard headers
#include <algorithm>
#include <vector>

// opencv headers
#include <opencv2/core.hpp>
using cv::Mat;
#include <opencv2/imgcodecs.hpp>
using cv::imread;

// classifier headers
#include "CharInfo.h"
#include "PlateCategory_SVM.h"
#include "PlateChar_SVM.h"
using Doit::CV::PlateRecogn::PlateCategory_SVM;
using Doit::CV::PlateRecogn::PlateCategory_t;
using Doit::CV::PlateRecogn::PlateCategory_tToString;
using Doit::CV::PlateRecogn::PlateChar_SVM;
using Doit::CV::PlateRecogn::PlateChar_t;
using Doit::CV::PlateRecogn::PlateChar_tToString;

#include "mainwindow.h"
#include "ui_mainwindow.h"

QString MainWindow::CharModelFileName = "CharModel.yaml";
QString MainWindow::CategoryModelFileName = "CategoryModel.yaml";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    for (auto &kernelString : kernelStrings) {
        ui->kernel_comboBox->addItem(kernelString);
    }
    ui->kernel_comboBox->setCurrentIndex(1);
    ui->C_spinBox->setValue(C);
    ui->gamma_spinBox->setValue(gamma);

    ui->trainingCount_label->setText("");
    ui->validationCount_label->setText("");
    ui->validationAccuracy_label->setText("");
    ui->timeCounter_label->setText("");
    ui->imageLabel->setText("");
    ui->predictedImageLabel->setText("");

    ui->filesSelection_comboBox->addItem("ALL_IMAGES");
    ui->filesSelection_comboBox->addItem("TRAINING_IMAGES");
    ui->filesSelection_comboBox->addItem("VALIDATION_IMAGES");
    ui->filesSelection_comboBox->addItem("CORRETC_PREDICTED_IMAGES");
    ui->filesSelection_comboBox->addItem("WRONG_PREDICTED_IMAGES");

    ui->allFiles->setViewMode(QListWidget::IconMode);

    worker = new TrainWorker(this);
    trainingThread = new QThread;

    worker->moveToThread(trainingThread);
    connect(trainingThread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), this, SLOT(after_trainingCompleting()));
    connect(worker, SIGNAL(showOverwriteModel_messageBox()), this,
            SLOT(showOverwriteModel_messageBox()),
            Qt::ConnectionType::BlockingQueuedConnection);
    connect(worker, SIGNAL(prediction_Completed()), this,
            SLOT(prediction_Completed()));

    connect(ui->openPlateChar, SIGNAL(triggered()), this,
            SLOT(on_openCharFolder()));
    connect(ui->openPlateCategory, SIGNAL(triggered()), this,
            SLOT(on_openCategoryFolder()));
}

MainWindow::~MainWindow() {
    delete ui;
    delete worker;
    delete trainingThread;
}

void MainWindow::reset(){
    images.clear();
    paths.clear();
    tags.clear();
    Hogs.clear();

    trainingIndices.clear();
    validationIndices.clear();
    validationPrediction.clear();
    correctValidationIndices.clear();
    wrongValidationIndices.clear();
}

void MainWindow::on_openCharFolder() {
    mode = PLATE_CHAR;
    iconSize = {50, 50};
    detailSize = {100, 100};
    HOGWinsize = cv::Size(16, 32);
    reset();

    QString trainingRoot = QFileDialog::getExistingDirectory(
        this, QString::fromLocal8Bit("选择训练样本根目录"), "../../../bin/platecharsamples/chars");
    if (trainingRoot == "") {
        return;
    }
    QDir root = QDir(trainingRoot);
    for (auto it = begin(PlateChar_tToString); it < end(PlateChar_tToString);
         ++it) {
        QString name = QString::fromLocal8Bit(*it);
        QString charPathName = trainingRoot + DIRECTORY_DELIMITER + name;
        QDir charDir = QDir(charPathName);
        QStringList charImagePaths = charDir.entryList(QStringList() << "*.jpg"
                                                                     << "*.png",
                                                       QDir::Files);

        int tag = it - begin(PlateChar_tToString);

        for (auto &file : charImagePaths) {
            QString filePath = charPathName + DIRECTORY_DELIMITER + file;
            images.push_back(imread(filePath.toLocal8Bit().toStdString()));
            paths.push_back(filePath);
            tags.push_back(tag);
        }
        qDebug() << QString::fromLocal8Bit(*it) << ", " << charPathName
                 << ", count:" << charImagePaths.size();
    }

    splitDataSet();
    showLoadedImages();
}

void MainWindow::on_openCategoryFolder() {
    mode = PLATE_CATEGORY;
    iconSize = {100, 50};
    detailSize = {250, 100};
    HOGWinsize = cv::Size(96, 32);
    reset();


    QString trainingRoot = QFileDialog::getExistingDirectory(
        this, QString::fromLocal8Bit("选择训练样本根目录"), "../../../bin/platecharsamples/plates");
    if (trainingRoot == "") {
        return;
    }
    QDir root = QDir(trainingRoot);
    for (auto it = begin(PlateCategory_tToString);
         it < end(PlateCategory_tToString); ++it) {
        QString name = QString::fromLocal8Bit(*it);
        QString categoryPathName = trainingRoot + DIRECTORY_DELIMITER + name;
        QDir categoryDir = QDir(categoryPathName);
        QStringList categoryImagePaths =
            categoryDir.entryList(QStringList() << "*.jpg"
                                                << "*.png",
                                  QDir::Files);

        int tag = it - begin(PlateCategory_tToString);

        for (auto &file : categoryImagePaths) {
            QString filePath = categoryPathName + DIRECTORY_DELIMITER + file;
            images.push_back(imread(filePath.toLocal8Bit().toStdString()));
            paths.push_back(filePath);
            tags.push_back(tag);
        }
        qDebug() << QString::fromLocal8Bit(*it) << ", " << categoryPathName
                 << ", count:" << categoryImagePaths.size();
    }

    splitDataSet();
    showLoadedImages();
}

void MainWindow::on_filesSelection_comboBox_currentIndexChanged(int index) {
    showLoadedImages();
}

void MainWindow::splitDataSet(float trainingRatio) {
    int sampleCount = images.size();
    auto randomIndices = std::vector<size_t>(sampleCount);
    iota(randomIndices.begin(), randomIndices.end(), 0);
    random_shuffle(randomIndices.begin(), randomIndices.end());

    int trainingCount = sampleCount * trainingRatio;
    trainingIndices.clear();
    validationIndices.clear();
    trainingIndices.assign(randomIndices.begin(),
                           randomIndices.begin() + trainingCount);
    validationIndices.assign(randomIndices.begin() + trainingCount,
                             randomIndices.end());
}

void MainWindow::showLoadedImages() {
    ui->allFiles->clear();
    int currentMode = ui->filesSelection_comboBox->currentIndex();
    switch (currentMode) {
    case 0:
        showAllImages();
        break;
    case 1:
        showTrainingImages();
        break;
    case 2:
        showValidationImages();
        break;
    case 3:
        showCorrectValidationImages();
        break;
    case 4:
        showWrongValidationImages();
        break;
    default:
        qDebug() << "not recognized mode";
    }
}

void MainWindow::showAllImages() {
    for (size_t i = 0; i < images.size(); ++i) {
        QIcon icon = QIcon(
            QPixmap::fromImage(Mat2QImage(images[i], QImage::Format_RGB888)).scaled(iconSize));
        
        QString tagString = mode == PLATE_CHAR
                                ? QString::fromLocal8Bit(PlateChar_tToString[tags[i]])
                                : QString::fromLocal8Bit(PlateCategory_tToString[tags[i]]);
        QListWidgetItem *item = new QListWidgetItem(icon, tagString);
        ui->allFiles->addItem(item);
    }
}
void MainWindow::showTrainingImages() {
    for (auto i : trainingIndices) {
        QIcon icon = QIcon(
            QPixmap::fromImage(Mat2QImage(images[i], QImage::Format_RGB888)).scaled(iconSize));
        QString tagString = mode == PLATE_CHAR
                                ? QString::fromLocal8Bit(PlateChar_tToString[tags[i]])
                                : QString::fromLocal8Bit(PlateCategory_tToString[tags[i]]);
        QListWidgetItem *item = new QListWidgetItem(icon, tagString);
        ui->allFiles->addItem(item);
    }
}
void MainWindow::showValidationImages() {
    for (auto i : validationIndices) {
        QIcon icon = QIcon(
            QPixmap::fromImage(Mat2QImage(images[i], QImage::Format_RGB888)).scaled(iconSize));
        QString tagString = mode == PLATE_CHAR
                                ? QString::fromLocal8Bit(PlateChar_tToString[tags[i]])
                                : QString::fromLocal8Bit(PlateCategory_tToString[tags[i]]);
        QListWidgetItem *item = new QListWidgetItem(icon, tagString);
        ui->allFiles->addItem(item);
    }
}
void MainWindow::showCorrectValidationImages() {
    if (validationPrediction.size() == 0) {
        return;
    }
    for (size_t i = 0; i < correctValidationIndices.size(); ++i) {
        int imageIndex = validationIndices[correctValidationIndices[i]];
        QIcon icon = QIcon(QPixmap::fromImage(
            Mat2QImage(images[imageIndex], QImage::Format_RGB888)).scaled(iconSize));
        QString tagString = mode == PLATE_CHAR
                                ? QString::fromLocal8Bit(PlateChar_tToString[tags[imageIndex]])
                                : QString::fromLocal8Bit(PlateCategory_tToString[tags[imageIndex]]);

        QListWidgetItem *item = new QListWidgetItem(icon, tagString);
        ui->allFiles->addItem(item);
    }
}
void MainWindow::showWrongValidationImages() {
    if (validationPrediction.size() == 0) {
        return;
    }
    for (size_t i = 0; i < wrongValidationIndices.size(); ++i) {
        int imageIndex = validationIndices[wrongValidationIndices[i]];
        QIcon icon = QIcon(QPixmap::fromImage(
            Mat2QImage(images[imageIndex], QImage::Format_RGB888)).scaled(iconSize));
        QString tagString = mode == PLATE_CHAR
                                ? QString::fromLocal8Bit(PlateChar_tToString[tags[imageIndex]])
                                : QString::fromLocal8Bit(PlateCategory_tToString[tags[imageIndex]]);

        QListWidgetItem *item = new QListWidgetItem(icon, tagString);
        ui->allFiles->addItem(item);
    }
}

QImage MainWindow::Mat2QImage(const cv::Mat &mat, QImage::Format format) {
    cv::Mat temp = mat.clone();

    if (mat.channels() == 3)
        cv::cvtColor(mat, temp, cv::COLOR_BGR2RGB);

    QImage image = QImage((const unsigned char *)(temp.data), temp.cols,
                          temp.rows, temp.cols * temp.channels(), format)
                       .copy();

    temp.release();

    return image;
}

using cv::Point;
using cv::Scalar;
using cv::Size;
Mat get_hogdescriptor_visu(const Mat &color_origImg,
                           vector<float> &descriptorValues, const Size &size);
void MainWindow::on_allFiles_currentItemChanged(QListWidgetItem *current,
                                                QListWidgetItem *previous) {
    QModelIndex selection = ui->allFiles->currentIndex();
    int vectorIndex = selection.row();
    if(vectorIndex == -1)
        return;
    int currentMode = ui->filesSelection_comboBox->currentIndex();
    switch (currentMode) {
    case 0:
        showAllImagesDetail(vectorIndex);
        break;
    case 1:
        showTrainingImagesDetail(vectorIndex);
        break;
    case 2:
        showValidationImagesDetail(vectorIndex);
        break;
    case 3:
        showCorrectValidationImagesDetail(vectorIndex);
        break;
    case 4:
        showWrongValidationImagesDetail(vectorIndex);
        break;
    default:
        qDebug() << "not recognized mode";
    }
}

void MainWindow::showAllImagesDetail(int index) {
    Mat image = images[index];
    QString path = paths[index];
    int tag = tags[index];

    ui->originalImage->setPixmap(
        QPixmap::fromImage(Mat2QImage(image, QImage::Format_RGB888))
            .scaled(detailSize));

    std::vector<float> hog;
    if(mode == PLATE_CHAR)
        hog = PlateChar_SVM::ComputeHogDescriptors(image);
    else
        hog = PlateCategory_SVM::ComputeHogDescriptors(image);

    Mat hogMat = get_hogdescriptor_visu(image, hog, HOGWinsize);
    ui->HOGImage->setPixmap(
        QPixmap::fromImage(Mat2QImage(hogMat, QImage::Format_RGB888))
            .scaled(detailSize));
    QString tagString = mode == PLATE_CHAR ? QString::fromLocal8Bit(PlateChar_tToString[tag]) : QString::fromLocal8Bit(PlateCategory_tToString[tag]);
    ui->imageLabel->setText(tagString);
    ui->predictedImageLabel->setText("");
    ui->predictedImageLabel->setEnabled(false);
}
void MainWindow::showTrainingImagesDetail(int index) {
    index = trainingIndices[index];
    Mat image = images[index];
    QString path = paths[index];
    int tag = tags[index];

    ui->originalImage->setPixmap(
        QPixmap::fromImage(Mat2QImage(image, QImage::Format_RGB888))
            .scaled(detailSize));

    std::vector<float> hog;
    if(mode == PLATE_CHAR)
        hog = PlateChar_SVM::ComputeHogDescriptors(image);
    else
        hog = PlateCategory_SVM::ComputeHogDescriptors(image);

    Mat hogMat = get_hogdescriptor_visu(image, hog, HOGWinsize);
    ui->HOGImage->setPixmap(
        QPixmap::fromImage(Mat2QImage(hogMat, QImage::Format_RGB888))
            .scaled(detailSize));
    QString tagString = mode == PLATE_CHAR ? QString::fromLocal8Bit(PlateChar_tToString[tag]) : QString::fromLocal8Bit(PlateCategory_tToString[tag]);            
    ui->imageLabel->setText(tagString);
    ui->predictedImageLabel->setText("");
    ui->predictedImageLabel->setEnabled(false);
}
void MainWindow::showValidationImagesDetail(int index) {
    int imageIndex = validationIndices[index];
    Mat image = images[imageIndex];
    QString path = paths[imageIndex];
    int tag = tags[imageIndex];
    int predictedTag = validationPrediction.size() > 0 ? validationPrediction[index] : -1;

    ui->originalImage->setPixmap(
        QPixmap::fromImage(Mat2QImage(image, QImage::Format_RGB888))
            .scaled(detailSize));

    std::vector<float> hog;
    if(mode == PLATE_CHAR)
        hog = PlateChar_SVM::ComputeHogDescriptors(image);
    else
        hog = PlateCategory_SVM::ComputeHogDescriptors(image);

    Mat hogMat = get_hogdescriptor_visu(image, hog, HOGWinsize);
    ui->HOGImage->setPixmap(
        QPixmap::fromImage(Mat2QImage(hogMat, QImage::Format_RGB888))
            .scaled(detailSize));
    QString tagString = mode == PLATE_CHAR ? QString::fromLocal8Bit(PlateChar_tToString[tag]) : QString::fromLocal8Bit(PlateCategory_tToString[tag]);      
    QString predictedTagString;
    if(predictedTag == -1)
        predictedTagString = "";    
    else
        predictedTagString = mode == PLATE_CHAR ? QString::fromLocal8Bit(PlateChar_tToString[predictedTag]) : QString::fromLocal8Bit(PlateCategory_tToString[predictedTag]);      
    ui->imageLabel->setText(tagString);
    ui->predictedImageLabel->setText(predictedTagString);
    ui->predictedImageLabel->setEnabled(true);
}
void MainWindow::showCorrectValidationImagesDetail(int index) {
    int imageIndex = validationIndices[correctValidationIndices[index]];
    Mat image = images[imageIndex];
    QString path = paths[imageIndex];
    int tag = tags[imageIndex];
    int predictedTag = validationPrediction[correctValidationIndices[index]];

    ui->originalImage->setPixmap(
        QPixmap::fromImage(Mat2QImage(image, QImage::Format_RGB888))
            .scaled(detailSize));

    std::vector<float> hog;
    if(mode == PLATE_CHAR)
        hog = PlateChar_SVM::ComputeHogDescriptors(image);
    else
        hog = PlateCategory_SVM::ComputeHogDescriptors(image);

    Mat hogMat = get_hogdescriptor_visu(image, hog, HOGWinsize);
    ui->HOGImage->setPixmap(
        QPixmap::fromImage(Mat2QImage(hogMat, QImage::Format_RGB888))
            .scaled(detailSize));
    QString tagString = mode == PLATE_CHAR ? QString::fromLocal8Bit(PlateChar_tToString[tag]) : QString::fromLocal8Bit(PlateCategory_tToString[tag]);            
    QString predictedTagString = mode == PLATE_CHAR ? QString::fromLocal8Bit(PlateChar_tToString[predictedTag]) : QString::fromLocal8Bit(PlateCategory_tToString[predictedTag]);          
    ui->imageLabel->setText(tagString);
    ui->predictedImageLabel->setText(predictedTagString);
    ui->predictedImageLabel->setEnabled(true);
}
void MainWindow::showWrongValidationImagesDetail(int index) {
    int imageIndex = validationIndices[wrongValidationIndices[index]];
    Mat image = images[imageIndex];
    QString path = paths[imageIndex];
    int tag = tags[imageIndex];
    int predictedTag = validationPrediction[wrongValidationIndices[index]];

    ui->originalImage->setPixmap(
        QPixmap::fromImage(Mat2QImage(image, QImage::Format_RGB888))
            .scaled(detailSize));

    std::vector<float> hog;
    if(mode == PLATE_CHAR)
        hog = PlateChar_SVM::ComputeHogDescriptors(image);
    else
        hog = PlateCategory_SVM::ComputeHogDescriptors(image);

    Mat hogMat = get_hogdescriptor_visu(image, hog, HOGWinsize);
    ui->HOGImage->setPixmap(
        QPixmap::fromImage(Mat2QImage(hogMat, QImage::Format_RGB888))
            .scaled(detailSize));
    QString tagString = mode == PLATE_CHAR ? QString::fromLocal8Bit(PlateChar_tToString[tag]) : QString::fromLocal8Bit(PlateCategory_tToString[tag]);            
    QString predictedTagString = mode == PLATE_CHAR ? QString::fromLocal8Bit(PlateChar_tToString[predictedTag]) : QString::fromLocal8Bit(PlateCategory_tToString[predictedTag]);          
    ui->imageLabel->setText(tagString);
    ui->predictedImageLabel->setText(predictedTagString);
    ui->predictedImageLabel->setEnabled(true);
}

// following code is from
// http://www.juergenbrauer.org/old_wiki/doku.php?id=public:hog_descriptor_computation_and_visualization
Mat get_hogdescriptor_visu(const Mat &color_origImg,
                           vector<float> &descriptorValues, const Size &size) {
    const int DIMX = size.width;
    const int DIMY = size.height;
    float zoomFac = 3;
    Mat visu;
    resize(color_origImg, visu,
           Size((int)(color_origImg.cols * zoomFac),
                (int)(color_origImg.rows * zoomFac)));

    int cellSize = 8;
    int gradientBinSize = 9;
    float radRangeForOneBin =
        (float)(CV_PI /
                (float)gradientBinSize); // dividing 180 into 9 bins, how large
                                         // (in rad) is one bin?

    // prepare data structure: 9 orientation / gradient strenghts for each cell
    int cells_in_x_dir = DIMX / cellSize;
    int cells_in_y_dir = DIMY / cellSize;
    float ***gradientStrengths = new float **[cells_in_y_dir];
    int **cellUpdateCounter = new int *[cells_in_y_dir];
    for (int y = 0; y < cells_in_y_dir; y++) {
        gradientStrengths[y] = new float *[cells_in_x_dir];
        cellUpdateCounter[y] = new int[cells_in_x_dir];
        for (int x = 0; x < cells_in_x_dir; x++) {
            gradientStrengths[y][x] = new float[gradientBinSize];
            cellUpdateCounter[y][x] = 0;

            for (int bin = 0; bin < gradientBinSize; bin++)
                gradientStrengths[y][x][bin] = 0.0;
        }
    }

    // nr of blocks = nr of cells - 1
    // since there is a new block on each cell (overlapping blocks!) but the
    // last one
    int blocks_in_x_dir = cells_in_x_dir - 1;
    int blocks_in_y_dir = cells_in_y_dir - 1;

    // compute gradient strengths per cell
    int descriptorDataIdx = 0;
    int cellx = 0;
    int celly = 0;

    for (int blockx = 0; blockx < blocks_in_x_dir; blockx++) {
        for (int blocky = 0; blocky < blocks_in_y_dir; blocky++) {
            // 4 cells per block ...
            for (int cellNr = 0; cellNr < 4; cellNr++) {
                // compute corresponding cell nr
                cellx = blockx;
                celly = blocky;
                if (cellNr == 1)
                    celly++;
                if (cellNr == 2)
                    cellx++;
                if (cellNr == 3) {
                    cellx++;
                    celly++;
                }

                for (int bin = 0; bin < gradientBinSize; bin++) {
                    float gradientStrength =
                        descriptorValues[descriptorDataIdx];
                    descriptorDataIdx++;

                    gradientStrengths[celly][cellx][bin] += gradientStrength;

                } // for (all bins)

                // note: overlapping blocks lead to multiple updates of this
                // sum! we therefore keep track how often a cell was updated, to
                // compute average gradient strengths
                cellUpdateCounter[celly][cellx]++;

            } // for (all cells)

        } // for (all block x pos)
    }     // for (all block y pos)

    // compute average gradient strengths
    for (celly = 0; celly < cells_in_y_dir; celly++) {
        for (cellx = 0; cellx < cells_in_x_dir; cellx++) {

            float NrUpdatesForThisCell = (float)cellUpdateCounter[celly][cellx];

            // compute average gradient strenghts for each gradient bin
            // direction
            for (int bin = 0; bin < gradientBinSize; bin++) {
                gradientStrengths[celly][cellx][bin] /= NrUpdatesForThisCell;
            }
        }
    }

    // draw cells
    for (celly = 0; celly < cells_in_y_dir; celly++) {
        for (cellx = 0; cellx < cells_in_x_dir; cellx++) {
            int drawX = cellx * cellSize;
            int drawY = celly * cellSize;

            int mx = drawX + cellSize / 2;
            int my = drawY + cellSize / 2;

            rectangle(visu,
                      Point((int)(drawX * zoomFac), (int)(drawY * zoomFac)),
                      Point((int)((drawX + cellSize) * zoomFac),
                            (int)((drawY + cellSize) * zoomFac)),
                      Scalar(100, 100, 100), 1);

            // draw in each cell all 9 gradient strengths
            for (int bin = 0; bin < gradientBinSize; bin++) {
                float currentGradStrength =
                    gradientStrengths[celly][cellx][bin];

                // no line to draw?
                if (currentGradStrength == 0)
                    continue;

                float currRad = bin * radRangeForOneBin + radRangeForOneBin / 2;

                float dirVecX = cos(currRad);
                float dirVecY = sin(currRad);
                float maxVecLen = (float)(cellSize / 2.f);
                float scale =
                    2.5; // just a visualization scale, to see the lines better

                // compute line coordinates
                float x1 =
                    mx - dirVecX * currentGradStrength * maxVecLen * scale;
                float y1 =
                    my - dirVecY * currentGradStrength * maxVecLen * scale;
                float x2 =
                    mx + dirVecX * currentGradStrength * maxVecLen * scale;
                float y2 =
                    my + dirVecY * currentGradStrength * maxVecLen * scale;

                // draw gradient visualization
                line(visu, Point((int)(x1 * zoomFac), (int)(y1 * zoomFac)),
                     Point((int)(x2 * zoomFac), (int)(y2 * zoomFac)),
                     Scalar(0, 255, 0), 1);

            } // for (all bins)

        } // for (cellx)
    }     // for (celly)

    // don't forget to free memory allocated by helper data structures!
    for (int y = 0; y < cells_in_y_dir; y++) {
        for (int x = 0; x < cells_in_x_dir; x++) {
            delete[] gradientStrengths[y][x];
        }
        delete[] gradientStrengths[y];
        delete[] cellUpdateCounter[y];
    }
    delete[] gradientStrengths;
    delete[] cellUpdateCounter;

    return visu;

} // get_hogdescriptor_visu

void MainWindow::on_startTrainButton_clicked() {
    ui->startTrainButton->setEnabled(false);
    ui->startTrainButton->setText(QString::fromLocal8Bit("正在训练..."));
    trainingThread->start();
}

void MainWindow::after_trainingCompleting() {
    ui->startTrainButton->setText(QString::fromLocal8Bit("开始训练"));
    ui->startTrainButton->setEnabled(true);
    trainingThread->exit();
}

bool MainWindow::showOverwriteModel_messageBox() {
    QMessageBox *msgBox = new QMessageBox;
    msgBox->setText(QString::fromLocal8Bit("模型文件已经存在，是否覆盖？"));
    msgBox->setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    int ret = msgBox->exec();
    return ret == QMessageBox::Cancel;
}

void MainWindow::on_kernel_comboBox_currentIndexChanged(int index) {
    auto currentKernel = candidateKernels[index];
    if (currentKernel == SVM::KernelTypes::POLY) {
        ui->degree_label->setEnabled(true);
        ui->degree_spinBox->setEnabled(true);
    } else {
        ui->degree_label->setEnabled(false);
        ui->degree_spinBox->setEnabled(false);
    }
}

// void MainWindow::on_allTestFiles_currentItemChanged(QListWidgetItem *current,
//                                                     QListWidgetItem
//                                                     *previous) {
//     QModelIndex selection = ui->allTestFiles->currentIndex();
//     int vectorIndex = selection.row();
//     Mat image = images[validationIndices[vectorIndex]];
//     QString path = paths[validationIndices[vectorIndex]];
//     PlateChar_t tag = tags[validationIndices[vectorIndex]];
//     int predictedTag = validationPrediction[vectorIndex];

//     ui->testOriginalImage->setPixmap(
//         QPixmap::fromImage(Mat2QImage(image, QImage::Format_RGB888))
//             .scaled(50, 100));
//     auto hog = PlateChar_SVM::ComputeHogDescriptors(image);
//     Mat hogMat = get_hogdescriptor_visu(image, hog, Size{16, 32});
//     ui->testHOGImage->setPixmap(
//         QPixmap::fromImage(Mat2QImage(hogMat, QImage::Format_RGB888))
//             .scaled(50, 100));
//     ui->testImageLabel->setText(PlateChar_tToString[static_cast<size_t>(tag)]);
//     ui->predictedTestLabel->setText(QString::fromLocal8Bit(PlateChar_tToString[predictedTag]));
// }

// void MainWindow::showValidations() {
//     ui->allTestFiles->clear();
//     ui->allTestFiles->setViewMode(QListWidget::IconMode);
//     for (size_t i = 0; i < validationIndices.size(); ++i) {
//         QIcon icon = QIcon(QPixmap::fromImage(
//             Mat2QImage(images[validationIndices[i]],
//             QImage::Format_RGB888)));
//         QListWidgetItem *item =
//             new QListWidgetItem(icon,
//             PlateChar_tToString[static_cast<size_t>(
//                                           tags[validationIndices[i]])]);
//         ui->allTestFiles->addItem(item);
//     }
// }

void MainWindow::prediction_Completed() {
    // showValidations();
}

// Training worker
void TrainWorker::process() {
    train(mainWindow);
    emit finished();
}

void TrainWorker::train(MainWindow *mainWindow) {
    mainWindow->C = mainWindow->ui->C_spinBox->value();
    mainWindow->gamma = mainWindow->ui->gamma_spinBox->value();
    mainWindow->kernel =
        mainWindow
            ->candidateKernels[mainWindow->ui->kernel_comboBox->currentIndex()];
    if (mainWindow->kernel == SVM::KernelTypes::POLY)
        mainWindow->degree = mainWindow->ui->degree_spinBox->value();

    if(mainWindow->mode == MainWindow::PLATE_CHAR){
        if (QFile::exists(mainWindow->CharModelFileName)) {
            if (emit showOverwriteModel_messageBox()) {
                return;
            }
            QFile::remove(mainWindow->CharModelFileName);
        }
    } else{
        if (QFile::exists(mainWindow->CategoryModelFileName)) {
            if (emit showOverwriteModel_messageBox()) {
                return;
            }
            QFile::remove(mainWindow->CategoryModelFileName);
        }
    }

    if (mainWindow->images.size() == 0) {
        return;
    }

    QTime timeCounter;
    timeCounter.start();

    // PlateChar_SVM classifier;
    // decltype(PlateChar_SVM::ComputeHogDescriptors) *computeHog = mainWindow->mode == MainWindow::PLATE_CHAR ? PlateChar_SVM::ComputeHogDescriptors : PlateCategory_SVM::ComputeHogDescriptors;
    // decltype(PlateChar_SVM::Train) *train = mainWindow->mode == MainWindow::PLATE_CHAR ? PlateChar_SVM::Train : PlateCategory_SVM::Train;
    // decltype(PlateChar_SVM::Test) *test = mainWindow->mode == MainWindow::PLATE_CHAR ? PlateChar_SVM::Test : PlateCategory_SVM::Test;

    if (mainWindow->Hogs.size() == 0) {
        mainWindow->Hogs.clear();
        for (auto &image : mainWindow->images) {
            std::vector<float> Hog;
            if(mainWindow->mode == MainWindow::PLATE_CHAR)
                Hog = PlateChar_SVM::ComputeHogDescriptors(image);
            else
                Hog = PlateCategory_SVM::ComputeHogDescriptors(image);
            mainWindow->Hogs.push_back(Hog);
        }
    }

    int trainingCount = mainWindow->trainingIndices.size(),
        validationCount = mainWindow->validationIndices.size();

    int hogSize = mainWindow->Hogs[0].size();
    Mat training_data(trainingCount, hogSize, CV_32FC1, Scalar::all(0));
    Mat training_tag(trainingCount, 1, CV_32S);
    for (int i = 0; i < trainingCount; ++i) {
        Mat(1, hogSize, CV_32FC1,
            mainWindow->Hogs[mainWindow->trainingIndices[i]].data())
            .copyTo(training_data.row(i));
        training_tag.at<int>(i) =
            static_cast<int>(mainWindow->tags[mainWindow->trainingIndices[i]]);
    }

    if(mainWindow->mode == MainWindow::PLATE_CHAR)
        PlateChar_SVM::Train(training_data, training_tag, mainWindow->kernel,
                     mainWindow->C, mainWindow->gamma, mainWindow->degree);
    else
        PlateCategory_SVM::Train(training_data, training_tag, mainWindow->kernel,
                     mainWindow->C, mainWindow->gamma, mainWindow->degree);
                    
    vector<int> validationPrediction = {};
    mainWindow->correctValidationIndices.clear();
    mainWindow->wrongValidationIndices.clear();
    int trueCount = 0;
    for (int i = 0; i < validationCount; ++i) {
        Mat image = mainWindow->images[mainWindow->validationIndices[i]];
        int prediction;
        if(mainWindow->mode == MainWindow::PLATE_CHAR)
            prediction = static_cast<int>(PlateChar_SVM::Test(image));
        else
            prediction = static_cast<int>(PlateCategory_SVM::Test(image));

        validationPrediction.push_back(prediction);
        if (prediction ==
            static_cast<int>(
                mainWindow->tags[mainWindow->validationIndices[i]])) {
            ++trueCount;
            mainWindow->correctValidationIndices.push_back(i);
        } else {
            mainWindow->wrongValidationIndices.push_back(i);
        }
    }

    float accuracy = float(trueCount) / validationCount;

    int mss = timeCounter.elapsed();

    mainWindow->ui->trainingCount_label->setText(
        QString::number(trainingCount));
    mainWindow->ui->validationCount_label->setText(
        QString::number(validationCount));
    mainWindow->ui->validationAccuracy_label->setText(
        QString::number(accuracy * 100) + " %");
    mainWindow->ui->timeCounter_label->setText(QString::number(mss));

    if(mainWindow->mode == MainWindow::PLATE_CHAR)
        PlateChar_SVM::Save(MainWindow::CharModelFileName.toStdString());
    else
        PlateCategory_SVM::Save(MainWindow::CategoryModelFileName.toStdString());
    mainWindow->validationPrediction = validationPrediction;
    emit prediction_Completed();
}
