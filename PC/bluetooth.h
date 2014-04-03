#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QSerialPort>
#include <QObject>

class bluetooth : public QObject
{

    Q_OBJECT

private:
    QSerialPort *serialport;

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
