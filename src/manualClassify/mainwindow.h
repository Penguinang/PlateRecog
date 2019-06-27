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
    void showCharImageByPath(QString charImagePath);
    void moveFile(QString fullSourceFileName,QString destinationPath);
    void showImages(int tag);
    void labelImage(QString imageFilename,QString tag);
    void labelCharImage(QString imageFilename,QString tag);
    void updateWindow();
    void updateNumberWindow();
    void updateLetterWindow();
    void updateHanziWindow();
    void updateCharWindow();


private slots:

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_imageShowWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_actionopenFolder_triggered();

    void on_comboBox_number_currentTextChanged(const QString &arg1);

    void on_comboBox_char_currentTextChanged(const QString &arg1);

    void on_comboBox_chineseChar_currentTextChanged(const QString &arg1);

    void on_charListWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QString plateCategoryPath;
    QString charCategoryPath;
    QString basePath;
    QString plateSamplePath;
    QString charSamplepath;
};

#endif // MAINWINDOW_H
