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
    w.add_steering_data(10);
    QThread::sleep(1);
    w.add_steering_data(20);
    QThread::msleep(300);
    w.add_steering_data(0);
    QThread::sleep(1);
    w.add_steering_data(50);

    w.set_RFID("3BA00043562F");

    QByteArray test_array;
    test_array.resize(11);

    test_array[0] = 10;
    test_array[1] = 23;
    test_array[2] = 0;
    test_array[3] = 240;
    test_array[4] = 255;
    test_array[5] = 220;
    test_array[6] = 0;
    test_array[7] = 20;
    test_array[8] = 15;
    test_array[9] = 18;
    test_array[10] = 28;

    w.update_linesensor_plot(&test_array);


//    w.draw_next_point_steering(3);
//    QThread::sleep(1);
//    w.draw_next_point_steering(50);
//    QThread::msleep(50);
//    w.draw_next_point_steering(100);
//    for (long int i = 0; i < 4000000; ++i) {

//    }
//    w.draw_next_point_steering(1000);

    //draw_x_axis(scene_graph_1);

//    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();

//        qDebug() << list.at(1).portName();
//        qDebug() << list.at(2).portName();

//        bluetooth *connection = new bluetooth(list.at(1).portName(), &w);

//    QByteArray test;
//    test.resize(5);

//     test[0] = PKT_SPOOFED_REQUEST;
//     //test[1] = 4;
//     test[1] = 1;
//     test[2] = 1;
//     test[3] = 0x00;
//     test[4] = 0x02;

//     connection->open_port();
//     connection->write(test);

    return a.exec();
}
