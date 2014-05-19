#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QSerialPort>
#include <QObject>
#include <QByteArray>
#include <stdarg.h>

class MainWindow;

//#define send_packet(packet_id, ...) bluetooth::_send_packet(packet_id, sizeof((int[]){0,##__VA_ARGS__}) / sizeof(int), ##__VA_ARGS__)


class bluetooth : public QObject
{

    Q_OBJECT

private:
    QSerialPort *serialport;
    QByteArray current_packet;
    MainWindow *window;
    QByteArray buffer;

    void process_packet();

public:

    bluetooth(QString, MainWindow*);
    ~bluetooth();

    void write(QByteArray);

    bool open_port();

    void send_packet(char packet_id, int num_params, ...);

private slots:
    void handle_ready_read();
    void handle_error(QSerialPort::SerialPortError);
};

#endif // BLUETOOTH_H
