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

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    ../classifier/platecategory_svm.cpp \
    ../classifier/platechar_svm.cpp \
    ../classifier/charsegment_v3.cpp \
    ../classifier/utilities.cpp \
    ../classifier/platelocator_v3.cpp \
    ../classifier/PlateRecognition_V3.cpp

HEADERS += \
        mainwindow.h \
    ../classifier/charinfo.h \
    ../classifier/platelocator_v3.h \
    ../classifier/platecategory_svm.h \
    ../classifier/csharpimplementations.h \
    ../classifier/charsegment_v3.h \
    ../classifier/platechar_svm.h \
    ../classifier/utilities.h \
    #../classifier/PlateLocator_V3.h
    ../classifier/PlateRecognition_V3.h

FORMS += \
        mainwindow.ui

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
