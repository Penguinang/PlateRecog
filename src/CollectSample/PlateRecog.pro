#-------------------------------------------------
#
# Project created by QtCreator 2019-06-24T09:16:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlateRecog
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    picturepreparedia.cpp \
    ../classifier/PlateCategory_SVM.cpp \
    ../classifier/PlateChar_SVM.cpp \
    ../classifier/CharSegment_V3.cpp \
    ../classifier/Utilities.cpp \
    ../classifier/PlateLocator_V3.cpp

HEADERS += \
        mainwindow.h \
    ../classifier/CharInfo.h \
    picturepreparedia.h \
    ../classifier/PlateLocator_V3.h \
    ../classifier/PlateCategory_SVM.h \
    ../classifier/csharpImplementations.h \
    ../classifier/CharSegment_V3.h \
    ../classifier/PlateChar_SVM.h \
    ../classifier/Utilities.h \
    #../classifier/PlateLocator_V3.h

FORMS += \
        mainwindow.ui \
    picturepreparedia.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += D:/MySoftware/OPENCV/OpenCV-MinGW-Build-OpenCV-4.1.0-x64/include
INCLUDEPATH += ../classifier

LIBS += D:/MySoftware/OPENCV/OpenCV-MinGW-Build-OpenCV-4.1.0-x64/x64/mingw/bin/libopencv_core410.dll
LIBS += D:/MySoftware/OPENCV/OpenCV-MinGW-Build-OpenCV-4.1.0-x64/x64/mingw/bin/libopencv_highgui410.dll
LIBS += D:/MySoftware/OPENCV/OpenCV-MinGW-Build-OpenCV-4.1.0-x64/x64/mingw/bin/libopencv_imgproc410.dll
LIBS += D:/MySoftware/OPENCV/OpenCV-MinGW-Build-OpenCV-4.1.0-x64/x64/mingw/bin/libopencv_calib3d410.dll
LIBS += D:/MySoftware/OPENCV/OpenCV-MinGW-Build-OpenCV-4.1.0-x64/x64/mingw/bin/libopencv_imgcodecs410.dll
LIBS += D:/MySoftware/OPENCV/OpenCV-MinGW-Build-OpenCV-4.1.0-x64/x64/mingw/bin/libopencv_videoio410.dll
LIBS += D:/MySoftware/OPENCV/OpenCV-MinGW-Build-OpenCV-4.1.0-x64/x64/mingw/bin/libopencv_ml410.dll
LIBS += D:/MySoftware/OPENCV/OpenCV-MinGW-Build-OpenCV-4.1.0-x64/x64/mingw/bin/libopencv_objdetect410.dll

# For linux
INCLUDEPATH += /usr/include/opencv4
LIBS += opencv_highgui \
opencv_objdetect \
opencv_ml \
opencv_imgcodecs  \
opencv_imgproc \
opencv_core \
stdc++fs