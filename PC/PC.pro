#-------------------------------------------------
#
# Project created by QtCreator 2014-04-03T11:46:22
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += widgets

TARGET = PC
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bluetooth.cpp

HEADERS  += mainwindow.h \
    ../shared/packets.h \
    bluetooth.h

FORMS    += mainwindow.ui

CONFIG += console
