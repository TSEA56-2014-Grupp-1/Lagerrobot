#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QSerialPort>
#include <QObject>
#include <QByteArray>

class bluetooth : public QObject
{

    Q_OBJECT

private:
    QSerialPort *serialport;
    QByteArray current_packet;
public:

    bluetooth(QString);
    ~bluetooth();

    void write(QByteArray);
    QString open_port();


private slots:
    void handle_ready_read();
    void handle_error(QSerialPort::SerialPortError);
};

#endif // BLUETOOTH_H
