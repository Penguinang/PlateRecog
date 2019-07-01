#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QFileDialog>
#include <QDir>
#include <QStringList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QTabWidget>
#include <QWidget>
#include <QTime>
#include <QMessageBox>
#include <QCloseEvent>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "PlateLocator_v3.h"
#include "CharInfo.h"
#include "CharSegment_v3.h"
#include "PlateCategory_SVM.h"
#include "PlateChar_SVM.h"
#include "PlateRecognition_V3.h"

#include <QDebug>

#include <vector>
using std::vector;
#include <string>
using std::string;
using std::get;

using namespace Doit::CV::PlateRecogn;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString pathSelected;
    void showMat(cv::Mat mat,vector<tuple<vector<PlateInfo>, Mat, Mat, vector<vector<Point>>, Mat>> plates);
    QImage Mat2QImage(cv::Mat mat, QImage::Format format);
    QLabel* generateImageLabel(cv::Mat mat,QImage::Format format);
    void showPlateSplit(cv::Mat mat, vector<PlateInfo> plateInfos);
    void showPlateCharSplit(cv::Mat mat, vector<CharInfo> charInfos);
    void saveMatPic(Mat mat,string dir);
    Mat QImageToMat(QImage image);
    Mat QPixmapToMat(QPixmap pixmap);
    void saveAllPlateLocated();
    void saveAllPlateCharSplited();
    void showMatForAutoSample(Mat mat);
    void showAllPicturesGotten(QListWidgetItem *item);
    void closeEvent(QCloseEvent *event);

private slots:
    void selectDir();


    void on_fileList_itemClicked(QListWidgetItem *item);
    void on_plateList_itemClicked(QListWidgetItem *item);
    void on_savePlate_clicked();
    void on_saveChar_clicked();
    void on_charList_itemClicked(QListWidgetItem *item);
    void on_openFile_triggered();
    void on_autoCreateSample_triggered();
    void on_refresh_clicked();
    void on_checkSample_triggered();
    void on_actionTrain_triggered();
    void on_pushButton_clicked();
    void on_exit_triggered();
};

#endif // MAINWINDOW_H
