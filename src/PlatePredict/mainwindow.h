#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <QLabel>

#include "../classifier/CharInfo.h"
#include "../classifier/PlateCategory_SVM.h"
#include "../classifier/PlateChar_SVM.h"

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
    QLabel* generateImageLabel(cv::Mat mat, QImage::Format format);
    void imageArea(cv::Mat mat);
    void generatePlateRegion(PlateInfo plateInfo,int index,int duration);
    void generateCharRegion(PlateInfo plateInfo,int index);
    void generateProcessRegion(PlateInfo plateInfo, Mat originImage);

private:
    Ui::MainWindow *ui;
    QString pathSelected;
    QString currentImagePath;
    QString plateModelPath;
    QString charModelPath;
    PlateCategory_SVM plateClassifier;
    PlateChar_SVM charClassifier;
    bool state;


private slots:
    void selectDir();
    void selectPlateModel();
    void selectCharModel();
    void recog();
    void on_fileList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
};

#endif // MAINWINDOW_H
