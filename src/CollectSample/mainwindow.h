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

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "platelocator_v3.h"
#include "charinfo.h"
#include "charsegment_v3.h"
#include "PlateCategory_SVM.h"
#include "PlateChar_SVM.h"

#include <QDebug>

#include <vector>
using std::vector;
#include <string>
using std::string;

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
    void showMat(cv::Mat mat);
    QImage Mat2QImage(cv::Mat mat, QImage::Format format);
    QLabel* generateImageLabel(cv::Mat mat,QImage::Format format);
    void showPlateSplit(cv::Mat mat, vector<PlateInfo> plateInfos);
    void showPlateCharSplit(cv::Mat mat, vector<CharInfo> charInfos);
    void saveMatPic(Mat mat,string dir);
    Mat QImageToMat(QImage image);
    Mat QPixmapToMat(QPixmap pixmap);

private slots:
    void selectDir();


    void on_fileList_itemClicked(QListWidgetItem *item);
    void on_plateList_itemClicked(QListWidgetItem *item);
    void on_savePlate_clicked();
    void on_saveChar_clicked();
    void on_charList_itemClicked(QListWidgetItem *item);
};

#endif // MAINWINDOW_H
