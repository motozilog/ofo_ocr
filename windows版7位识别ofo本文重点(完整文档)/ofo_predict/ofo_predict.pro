#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T16:12:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ofo_predict
TEMPLATE = app
INCLUDEPATH += C:\QT\opencvok\include\opencv\
               C:\QT\opencvok\include\opencv2\
               C:\QT\opencvok\include

LIBS += -L C:\QT\opencvok\lib\libopencv_*.a

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
