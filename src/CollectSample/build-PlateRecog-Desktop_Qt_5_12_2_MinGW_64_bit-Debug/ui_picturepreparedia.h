/********************************************************************************
** Form generated from reading UI file 'picturepreparedia.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PICTUREPREPAREDIA_H
#define UI_PICTUREPREPAREDIA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_PicturePrepareDia
{
public:

    void setupUi(QDialog *PicturePrepareDia)
    {
        if (PicturePrepareDia->objectName().isEmpty())
            PicturePrepareDia->setObjectName(QString::fromUtf8("PicturePrepareDia"));
        PicturePrepareDia->resize(656, 523);

        retranslateUi(PicturePrepareDia);

        QMetaObject::connectSlotsByName(PicturePrepareDia);
    } // setupUi

    void retranslateUi(QDialog *PicturePrepareDia)
    {
        PicturePrepareDia->setWindowTitle(QApplication::translate("PicturePrepareDia", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PicturePrepareDia: public Ui_PicturePrepareDia {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PICTUREPREPAREDIA_H
