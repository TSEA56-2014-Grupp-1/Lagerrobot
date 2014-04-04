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

    w.draw_next_point_steering(3);
    QThread::sleep(1);
    w.draw_next_point_steering(50);
    QThread::msleep(500);
    w.draw_next_point_steering(50);
    for (long int i = 0; i < 4000000000; ++i) {

    }
    w.draw_next_point_steering(100);

//    QPen *pen = new QPen();
//    pen->setColor(Qt::black);
//    pen->setWidth(2);

//    QGraphicsTextItem *text;
//    QGraphicsLineItem *line;

//    line = w.scene_graph_1->addLine(0, 0, 200, 200, *pen);
//    text = w.scene_graph_1->addText("Test string");

    return a.exec();
}
