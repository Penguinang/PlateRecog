#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <QLabel>

#include "../classifier/CharInfo.h"

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
    void initSvm(QString plateModelPath,QString charModelPath);
    void generatePlateRegion(PlateInfo plateInfo,int index);
    void generateCharRegion(PlateInfo plateInfo,int index);
    void generateProcessRegion(PlateInfo plateInfo, Mat originImage);

private:
    Ui::MainWindow *ui;
    QString pathSelected;
    QString currentImagePath;
    QString plateModelPath = tr("C:/Users/rx/Desktop/PlateRecog/src/classifier/CategorySVM.yaml");
    QString charModelPath=tr("C:/Users/rx/Desktop/PlateRecog/src/classifier/CharSVM.yaml");
    bool state;


private slots:
    void selectDir();
    void selectPlateModel();
    void selectCharModel();
    void recog();
    void on_fileList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
};

#endif // MAINWINDOW_H
