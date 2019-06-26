#ifndef PICTUREPREPAREDIA_H
#define PICTUREPREPAREDIA_H

#include <QDialog>
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

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace Ui {
class PicturePrepareDia;
}

class PicturePrepareDia : public QDialog
{
    Q_OBJECT

public:
    explicit PicturePrepareDia(QWidget *parent = nullptr);
    ~PicturePrepareDia();

private:
    Ui::PicturePrepareDia *ui;
};

#endif // PICTUREPREPAREDIA_H
