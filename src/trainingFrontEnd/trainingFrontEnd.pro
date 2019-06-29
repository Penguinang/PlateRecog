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
DEFINES -= UNICODE

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
        ../classifier/PlateChar_SVM.cpp \
        ../classifier/PlateCategory_SVM.cpp \
        ../classifier/Utilities.cpp

HEADERS += \
        mainwindow.h \
# for classifier
        ../classifier/CharInfo.h \
        ../classifier/PlateChar_SVM.h \
        ../classifier/PlateCategory_SVM.h \
        ../classifier/Utilities.h

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
        INCLUDEPATH += D:/Applications/Tools/opencv4.1/opencv/build/include
        INCLUDEPATH += ../classifier/
#        LIBPATH =
        
#        LIBS += $${LIBPATH}/libopencv_core410.dll \
#                $${LIBPATH}/libopencv_highgui410.dll \
#                $${LIBPATH}/libopencv_imgproc410.dll \
#                $${LIBPATH}/libopencv_calib3d410.dll \
#                $${LIBPATH}/libopencv_imgcodecs410.dll \
#                $${LIBPATH}/libopencv_videoio410.dll \
#                $${LIBPATH}/libopencv_ml410.dll \
#                $${LIBPATH}/libopencv_objdetect410.dll

 QMAKE_CXXFLAGS_RELEASE += /O3 #/Od #/execution-charset:utf-8  #/utf-8 #/source-charset:utf-8

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../Tools/opencv4.1/opencv/build/x64/vc15/lib/ -lopencv_world410
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../Tools/opencv4.1/opencv/build/x64/vc15/lib/ -lopencv_world410d
else:unix:!macx: LIBS += -L$$PWD/../../../../../Tools/opencv4.1/opencv/build/x64/vc15/lib/ -lopencv_world410

INCLUDEPATH += $$PWD/../../../../../Tools/opencv4.1/opencv/build/x64/vc15
DEPENDPATH += $$PWD/../../../../../Tools/opencv4.1/opencv/build/x64/vc15

}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


