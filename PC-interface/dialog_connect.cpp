/*
 *      Class functions for dialog_connect class, will initiate a new connection.
 *
 *      @author Patrik Nyberg
 *
 */

#include "dialog_connect.h"
#include "ui_dialog_connect.h"
#include <QList>
#include <QSerialPortInfo>
#include <QDebug>
#include "mainwindow.h"

/*
 *      @brief Constructor for Dialog_connect, will create a list of all comports in the listWidget.
 *
 *      @param new_mainwindow The main window that the connection should be forwarded to.
 */
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

//XXX: TODO:
Dialog_connect::~Dialog_connect()
{
    delete ui;
}

/*
 *      @brief Callback for pushButton_cancel, will simply hide the window.
 */
void Dialog_connect::on_pushButton_cancel_clicked()
{
    this->hide();
}

/*
 *      @brief Callback for pushButton_connect, will forward the selection to mainwindow.
 */
void Dialog_connect::on_pushButton_connect_clicked()
{
    int i = ui->listWidget_connect->currentRow();
    mainwindow->connect_to_port(list.at(i).portName());
    this->hide();
}
