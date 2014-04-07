#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QSerialPort>
#include <QObject>

class MainWindow;

class bluetooth : public QObject
{

    Q_OBJECT

private:
    QSerialPort *serialport;
    MainWindow *window;

public:

    bluetooth(QString, MainWindow*);
    ~bluetooth();

    void write(QByteArray);
    void open_port();



private slots:
    void handle_ready_read();
    void handle_error(QSerialPort::SerialPortError);
};

#endif // BLUETOOTH_H
