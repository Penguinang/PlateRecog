#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QListWidgetItem>
#include <QButtonGroup>

#include <QDebug>
#include <QDir>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showImagesByPath(QString imagePath);
    void moveFile(QString fullSourceFileName,QString destinationPath);
    void showImages(int tag);
    void labelImage(QString imageFilename,int tag);
    void labelImage(QString imageFilename,QString tag);
    void updateWindow();


private slots:

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_imageShowWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_actionopenFolder_triggered();

private:
    Ui::MainWindow *ui;
    QString plateCategoryPath;
    QString charCategoryPath;
    QString plateSamplePath=QDir::currentPath()+"/samples/plates/";
};

#endif // MAINWINDOW_H
