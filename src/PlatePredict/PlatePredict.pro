#-------------------------------------------------
#
# Project created by QtCreator 2019-06-27T09:35:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlatePredict
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

INCLUDEPATH += D:/Applications/Tools/opencv4.1/opencv/build/include
INCLUDEPATH += ../classifier/
CONFIG += c++17

SOURCES += \
        ../classifier/CharInfo.cpp \
        ../classifier/CharSegment_V3.cpp \
        ../classifier/PlateCategory_SVM.cpp \
        ../classifier/PlateChar_SVM.cpp \
        ../classifier/PlateLocator_V3.cpp \
        ../classifier/PlateRecognition_V3.cpp \
        ../classifier/Utilities.cpp \
        ../classifier/debug.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        ../classifier/CharInfo.h \
        ../classifier/PlateCategory_SVM.h \
        ../classifier/PlateChar_SVM.h \
        ../classifier/PlateRecognition_V3.h \
        ../classifier/Utilities.h \
        ../classifier/csharpImplementations.h \
        ../classifier/debug.h \
        mainwindow.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc

RC_ICONS = start.ico

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../Tools/opencv4.1/opencv/build/x64/vc15/lib/ -lopencv_world410
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../Tools/opencv4.1/opencv/build/x64/vc15/lib/ -lopencv_world410d
else:unix: LIBS += -L$$PWD/../../../../../Tools/opencv4.1/opencv/build/x64/vc15/lib/ -lopencv_world410

INCLUDEPATH += $$PWD/../../../../../Tools/opencv4.1/opencv/build/x64/vc15
DEPENDPATH += $$PWD/../../../../../Tools/opencv4.1/opencv/build/x64/vc15
#msvc:QMAKE_CXXFLAGS_RELEASE += /source-charset:utf-8
