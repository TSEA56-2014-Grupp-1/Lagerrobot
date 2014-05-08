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

    return a.exec();
}
