#-------------------------------------------------
#
# Project created by QtCreator 2019-06-27T09:36:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = trainingFrontEnd
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

CONFIG += c++17 debug

SOURCES += \
        main.cpp \
        mainwindow.cpp \
# for classifier
        ../classifier/CharInfo.cpp \
        ../classifier/PlateChar_SVM.cpp

HEADERS += \
        mainwindow.h \
# for classifier
        ../classifier/CharInfo.h \
        ../classifier/PlateChar_SVM.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += ../classifier

unix{
        INCLUDEPATH += /usr/include/opencv4
        lib = opencv
        LIBS += -lopencv_highgui \
                -lopencv_objdetect \
                -lopencv_ml \
                -lopencv_imgcodecs  \
                -lopencv_imgproc \
                -lopencv_core \
                -lstdc++fs
}

win32{
        message("补充opencv的包含路径个库路径")
        INCLUDEPATH += 
        LIBPATH = 
        
        LIBS += $${LIBPATH}/libopencv_core410.dll \
                $${LIBPATH}/libopencv_highgui410.dll \
                $${LIBPATH}/libopencv_imgproc410.dll \
                $${LIBPATH}/libopencv_calib3d410.dll \
                $${LIBPATH}/libopencv_imgcodecs410.dll \
                $${LIBPATH}/libopencv_videoio410.dll \
                $${LIBPATH}/libopencv_ml410.dll \
                $${LIBPATH}/libopencv_objdetect410.dll
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
