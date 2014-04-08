/*
 *      Class functions for bluetooth class
 *
 *      @author Patrik Nyberg & Karl Linderhed
 *
 */
#include "bluetooth.h"
#include <QLabel>
#include <QDebug>
#include "mainwindow.h"
#include "../shared/packets.h"


QT_USE_NAMESPACE

void bluetooth::process_packet(char packet_id, QByteArray parameters)
{
    switch (packet_id) {
    case PKT_ARM_DECISION:

        break;
    case PKT_ARM_STATUS:

        break;
    case PKT_CHASSIS_DECISION:

        break;
    case PKT_CHASSIS_STATUS:

        break;
    case PKT_LINE_DATA:
        window->draw_next_point_steering(parameters[12]);
        break;
    case PKT_RANGE_DATA:

        break;
    case PKT_RFID_DATA:

        break;
    case PKT_SPOOFED_RESPONSE:
        window->print_on_log(QString::number(parameters[0], 16).append(QString::number(parameters[1], 16)));
        break;

    }
}

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

/*
 *      Will open the bluetooth port and set baudrate
 *
 *      @return True if port was opened successfully, false otherwise
 */
bool bluetooth::open_port() {
    if (serialport->open(QIODevice::ReadWrite)) {
        window->print_on_log("Bluetooth connected succesfully.");
        serialport->setBaudRate(QSerialPort::Baud115200);
        return true;
    }
    else {
        window->print_on_log("Error opening bluetooth");
        return false;
    }
}


void bluetooth::handle_ready_read() {

    //if (current_packet.isEmpty()) {
    //char packet_id;
    //serialport.read(&packet_id, 1);



    char packet_id;
    serialport->read(&packet_id, 1);
    //serialport->waitForReadyRead(1000);

    char num_parameters;
    serialport->read(&num_parameters, 1);
    //serialport->waitForReadyRead(1000);

    char temp_parameter;
    QByteArray parameters = QByteArray(num_parameters, 0);
    for (int i = 0; (serialport->read(&temp_parameter, 1) != 0) && i < num_parameters; ++i) {
        parameters[i] = temp_parameter;
        //serialport->waitForReadyRead(1000);
    }

    process_packet(packet_id, parameters);

}

/*
 *      Write data to the robot via bluetooth
 *
 *      @param data Data that will be written
 */
void bluetooth::write(QByteArray data) {
    serialport->write(data);
}

/*
 *      Sends a packet to the robot via bluetooth
 *
 *      @param packet_id ID of the packet
 *      @param num_args Number of arguments in the packet
 *      @param ... Arguments to be sent
 */
void bluetooth::send_packet(char packet_id, int num_args, ...) {
    va_list param_list;

    QByteArray parameter = QByteArray(num_args + 1, 0);

    parameter[0] = packet_id;

    va_start(param_list, num_args);

    for (int i = 1; i < num_args + 1; ++i) {
        parameter[i] = va_arg(param_list, int);
    }

    va_end(param_list);

    write(parameter);
}

/*
 *      SLOT that will be called when an error has occurred
 *
 *      @param error Type of error that has occurred
 */
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
//     else if (serialport->error() == QSerialPort::TimeoutError) {
//        window->print_on_log("Serialport timed out.");
//    }

    //Needed since this slot is called even though no error has occuured
    else if (serialport->error() == QSerialPort::NoError) {
        return;
    }
    else {
        window->print_on_log("Unknown error.");
    }
}




