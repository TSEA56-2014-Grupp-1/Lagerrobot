#include "bluetooth.h"
#include <QLabel>
#include <QDebug>
#include "mainwindow.h"

QT_USE_NAMESPACE

bluetooth::bluetooth(QString name, MainWindow* new_window)
{
    window = new_window;
    serialport = new QSerialPort(name);
    connect(serialport, SIGNAL(readyRead()), SLOT(handle_ready_read()));
    connect(serialport, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handle_error(QSerialPort::SerialPortError)));
}

bluetooth::~bluetooth() {
    serialport->close();
}

void bluetooth::open_port() {
    if (serialport->open(QIODevice::ReadWrite)) {
        window->print_on_log("Bluetooth connected succesfully.");
        serialport->setBaudRate(QSerialPort::Baud115200);
    }
    else
        window->print_on_log("Error opening bluetooth");
}


void bluetooth::handle_ready_read() {
    char packet_id;
    serialport->read(&packet_id, 1);
    char num_parameters;
    serialport->read(&num_parameters, 1);
    QByteArray parameters = serialport->read(num_parameters);

    qDebug() << "Packet id: "  << packet_id;
    qDebug() << "Number of params: " << num_parameters;

    for (int i = 0; i < parameters.length(); ++i) {
        qDebug() << parameters[i];
    }

}

void bluetooth::write(QByteArray data) {
    serialport->write(data);
}

void bluetooth::handle_error(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::WriteError) {
        window->print_on_log(QObject::tr("An I/O error occurred while writing data to port %1, error: %2").arg(serialport->portName()).arg(serialport->errorString()));
    }
    else if (error == QSerialPort::ReadError) {
        window->print_on_log(QObject::tr("An I/O error ocurred while reading data from port %1, error: %2").arg(serialport->portName()).arg(serialport->errorString()));
    }
    else if (serialport->error() == QSerialPort::DeviceNotFoundError){
        window->print_on_log("Device was not found.");
    }
    else if (serialport->error() == QSerialPort::PermissionError) {
        window->print_on_log("Permission denied.");
    }
    else if (serialport->error() == QSerialPort::OpenError) {
        window->print_on_log("Error while opening already openden device.");
    }
    else if (serialport->error() == QSerialPort::NoError) {
        return;
    }
    else {
        window->print_on_log("Unknown error.");
    }
}




