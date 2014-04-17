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

/*
 *      @brief Splices a QByteArray.
 *      @details Returns a QByteArray with values between startpos and endpos of the input array.
 *
 *      @param array The array that will be spliced.
 *      @param startpos The start position in the array.
 *      @param endpos The end position of the array.
 *
 *      @return QByteArray with the values between start position and end position from the input array.
 */
QByteArray splice(QByteArray array, int startpos, int endpos) {
    QByteArray result;
    result.resize(endpos-startpos);

    for (int i = startpos; i < endpos; ++i) {
        result[i - startpos] = array[i];
    }
    return result;

}

void bluetooth::process_packet()
{
    quint8 packet_id = buffer.at(0);
    QByteArray parameters = splice(buffer, 2, buffer.length());
    QString RFID = "";
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
        window->add_steering_data(parameters[12]);

        break;
    case PKT_RANGE_DATA:

        break;
    case PKT_RFID_DATA:
        for (int i = 1; i < 13; ++i) {
            RFID.append(parameters.at(i));
        }
        window->set_RFID(RFID);
        break;
    case PKT_CALIBRATION_DATA:
        window->print_on_log(QString("Calibration, new tape reference: ").append(QString::number(parameters[0])));
        break;
    case PKT_SPOOFED_RESPONSE:
        window->print_on_log(QString::number(parameters[0], 16).append(QString::number(parameters[1], 16)));
        break;

    }
}

/*
 *      @brief Constructor for the class bluetooth.
 *      @details Initiates window and serialport pointers and connects readyRead and error signals to their slots.
 *
 *      @param name Name of the QSerialPort.
 *      @param new_window Main window of the program.
 */
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
 *      @brief Will open the bluetooth port with correct settings.
 *      @details QSerialPort will be connected if possible, and settings will be set. Will return false if unable to connect.
 *
 *      @return True if port was opened successfully, false otherwise
 */
bool bluetooth::open_port() {
    if (serialport->open(QIODevice::ReadWrite)) {
        serialport->setBaudRate(QSerialPort::Baud115200);
        serialport->setFlowControl(QSerialPort::NoFlowControl);
        serialport->setDataBits(QSerialPort::Data8);
        serialport->setParity(QSerialPort::NoParity);
        serialport->setStopBits(QSerialPort::OneStop);
        return true;
    }
    else {
        return false;
    }
}

/*
 *      @brief Slot that will be called when bytes have been received from robot.
 *      @details Slot that will handle data from robot. If the whole message has been received it will call process_packet(). Otherwise it will read all bytes into buffer and return.
 */
void bluetooth::handle_ready_read() {
    QByteArray newBytes;

    if (buffer.isEmpty()) {
        if (serialport->bytesAvailable() > 2) {
            buffer = serialport->read(2); // read packet id and number of parameters
            newBytes = serialport->read(buffer.at(1));

            if (newBytes.length() < buffer.at(1)) {
                buffer += newBytes;
            }
            else {
                buffer += newBytes;
                process_packet();
                buffer.clear();
            }
        }
    }
    else {
        if (serialport->bytesAvailable() >= buffer.at(1)-(buffer.length()-2)) {
            buffer += serialport->read(buffer.at(1)-(buffer.length()-2));
            process_packet();
            buffer.clear();
        }
        else {
            buffer += serialport->readAll();
        }
    }

}

/*
 *      @brief Write data to the robot via bluetooth
 *
 *      @param data Data that will be written
 */
void bluetooth::write(QByteArray data) {
    serialport->write(data);
}

/*
 *      @brief Sends a packet to the robot via bluetooth
 *
 *      @param packet_id ID of the packet
 *      @param num_args Number of arguments in the packet
 *      @param ... Bytes to be sent
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
 *      @brief Slot that will be called when an error has occurred.
 *
 *      @param error Type of error that has occurred.
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




