#include "bluetooth.h"
#include <QLabel>
#include <QDebug>

QT_USE_NAMESPACE

bluetooth::bluetooth(QString name)
{
    serialport = new QSerialPort(name);
    connect(serialport, SIGNAL(readyRead()), SLOT(handle_ready_read()));
    connect(serialport, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handle_error(QSerialPort::SerialPortError)));
}

bluetooth::~bluetooth() {
    serialport->close();
}

QString bluetooth::open_port() {
    if (serialport->open(QIODevice::ReadWrite)) {
        return "Bluetooth connected succesfully.";
        serialport->setBaudRate(QSerialPort::Baud115200);
    }
    else
        return "Error opening bluetooth";
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
        // XXX: This should be printed in the log.
        qDebug() << QObject::tr("An I/O error occurred while writing data to port %1, error: %2").arg(serialport->portName()).arg(serialport->errorString()) << endl;
    }
    else if (error == QSerialPort::ReadError) {
        qDebug() << QObject::tr("An I/O error ocurred while reading data from port %1, error: %2").arg(serialport->portName()).arg(serialport->errorString()) << endl;
    }
    else if (serialport->error() == QSerialPort::DeviceNotFoundError){
        qDebug() << "Device was not found.";
    }
    else if (serialport->error() == QSerialPort::PermissionError) {
        qDebug() << "Permission denied.";
    }
    else if (serialport->error() == QSerialPort::OpenError) {
        qDebug() << "Error while opening already openden device.";
    }
    else {
        qDebug() << "Unknown error.";
    }
}



