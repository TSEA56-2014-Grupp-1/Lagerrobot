#include "mainwindow.h"
#include <QApplication>
#include <QThread>
#include <QSerialPortInfo>
#include "bluetooth.h"
#include "../shared/packets.h"
#include <QList>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setFocusPolicy(Qt::ClickFocus);
    w.show();
    w.activateWindow();
    w.raise();

    w.time->start();

//    w.draw_next_point_steering(3);
//    QThread::sleep(1);
//    w.draw_next_point_steering(50);
//    QThread::msleep(50);
//    w.draw_next_point_steering(100);
//    for (long int i = 0; i < 4000000; ++i) {

//    }
//    w.draw_next_point_steering(1000);

    //draw_x_axis(scene_graph_1);

    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();

        qDebug() << list.at(1).portName();
        qDebug() << list.at(2).portName();

        bluetooth *connection = new bluetooth(list.at(1).portName());

    QByteArray test;
    test.resize(5);

     test[0] = PKT_ARM_COMMAND;
     //test[1] = 4;
     test[1] = CMD_ARM_MOVE;
     test[2] = 1;
     test[3] = 0x34;
     test[4] = 0x12;

     connection->open_port();
     connection->write(test);

    return a.exec();
}
