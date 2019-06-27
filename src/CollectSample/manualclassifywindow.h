#ifndef MANUALCLASSIFYWINDOW_H
#define MANUALCLASSIFYWINDOW_H

#include <QMainWindow>

namespace  Ui{
    class ManualClassifyWindow;
}

class ManualClassifyWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit ManualClassifyWindow(QWidget *parent = nullptr);

    ~ManualClassifyWindow();
private:
    Ui::ManualClassifyWindow *ui;
};

#endif // MANUALCLASSIFYWINDOW_H
