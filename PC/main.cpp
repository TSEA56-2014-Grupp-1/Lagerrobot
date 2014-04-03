#include "mainwindow.h"
#include <QApplication>
#include <QSerialPortInfo>
#include "bluetooth.h"
#include <QList>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();

    qDebug() << list.at(1).portName();
    qDebug() << list.at(2).portName();

    bluetooth *connection = new bluetooth(list.at(2).portName());


    QByteArray test;
    test.resize(5);
    test[0] = 1;
    test[1] = 0;
    test[2] = 0x41;
    test[3] = 0x42;
    test[4] = 0x43;

    connection->open_port();
    //connection->write(test);

    return a.exec();
}
