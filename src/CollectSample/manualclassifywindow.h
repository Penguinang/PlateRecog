#ifndef MANUALCLASSIFYWINDOW_H
#define MANUALCLASSIFYWINDOW_H

#include <QMainWindow>

#include <QMainWindow>
#include <QListWidget>
#include <QListWidgetItem>
#include <QButtonGroup>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QFileDialog>

namespace  Ui{
    class ManualClassifyWindow;
}

class ManualClassifyWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit ManualClassifyWindow(QWidget *parent = nullptr);
    ~ManualClassifyWindow();

    void showImagesByPath(QString imagePath);
    void showCharImageByPath(QString charImagePath);
    void moveFile(QString fullSourceFileName,QString destinationPath);
    void showImages(int tag);
    void labelImage(QString imageFilename,int tag);
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

    void closeEvent(QCloseEvent *event);

private:
    Ui::ManualClassifyWindow *ui;
    QString plateCategoryPath;
    QString charCategoryPath;
    //QString plateSamplePath=QDir::currentPath()+"/samples/plates/";
    //QString charSamplepath=QDir::currentPath()+"/samples/chars/";
    QString basePath;
    QString plateSamplePath;
    QString charSamplepath;
};

#endif // MANUALCLASSIFYWINDOW_H
