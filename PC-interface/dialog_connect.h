#ifndef DIALOG_CONNECT_H
#define DIALOG_CONNECT_H

#include <QDialog>

class MainWindow;
class QSerialPortInfo;

namespace Ui {
class Dialog_connect;
}

class Dialog_connect : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_connect(MainWindow*,QWidget *parent = 0);
    ~Dialog_connect();


private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_connect_clicked();

private:

    Ui::Dialog_connect *ui;
    MainWindow* mainwindow;
    QList<QSerialPortInfo> list;
};

#endif // DIALOG_CONNECT_H
