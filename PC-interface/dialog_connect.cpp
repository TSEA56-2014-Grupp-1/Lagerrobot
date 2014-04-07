#include "dialog_connect.h"
#include "ui_dialog_connect.h"
#include <QList>
#include <QSerialPortInfo>
#include <QDebug>
#include "mainwindow.h"


Dialog_connect::Dialog_connect(MainWindow* new_mainwindow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_connect)
{
    ui->setupUi(this);
    mainwindow = new_mainwindow;
    list = QSerialPortInfo::availablePorts();
    for (int i = 0; i < list.size(); ++i) {
        ui->listWidget_connect->addItem(list.at(i).portName());
    }

}

Dialog_connect::~Dialog_connect()
{
    delete ui;
}

void Dialog_connect::on_pushButton_cancel_clicked()
{
    this->hide();
}



void Dialog_connect::on_pushButton_connect_clicked()
{
    int i = ui->listWidget_connect->currentRow();
    mainwindow->connect_to_port(list.at(i).portName());
    this->hide();
}
