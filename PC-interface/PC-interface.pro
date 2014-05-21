#-------------------------------------------------
#
# Project created by QtCreator 2014-04-01T12:59:01
#
#-------------------------------------------------

QT       += core gui
QT       += widgets
QT       += serialport
QT       += widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = PC-interface
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bluetooth.cpp \
    dialog_connect.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    bluetooth.h \
    ../shared/packets.h \
    dialog_connect.h \
    qcustomplot.h


FORMS    += mainwindow.ui \
    dialog_connect.ui
