#include "mainwindow.h"
#include <QApplication>

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
