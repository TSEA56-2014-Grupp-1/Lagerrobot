#include "mainwindow.h"
#include <QApplication>
#include <QThread>

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


    return a.exec();
}
