#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTime>
#include <QTimer>
#include <QShortcut>
#include <QKeySequence>
#include <QKeyEvent>
#include <QGraphicsScene>

class bluetooth;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     QTime *time_graph = new QTime();

    void print_on_log(QString);

    void new_connection(bluetooth*);

    void connect_to_port(QString);

	void add_mass_data(int);

    void add_steering_data(int);

    void set_RFID(QString);

	void update_linesensor_plot(QByteArray*parameters);

	void pickupstation(QByteArray*);

	void handle_decision(quint8 decision);

    void add_range_data(quint8 sensor, quint16 new_data);

    void disable_buttons();
    void enable_buttons();

private slots:
    void keyPressEvent(QKeyEvent *key_pressed);

	void keyReleaseEvent(QKeyEvent *key_released);

    void on_pushButton_forward_pressed();

    void on_pushButton_back_pressed();

    void on_pushButton_left_pressed();

    void on_pushButton_right_pressed();

    void on_pushButton_start_line_clicked();

    void on_pushButton_stop_line_clicked();

	void on_pushButton_base_right_released();

	void on_pushButton_base_right_pressed();

	void on_pushButton_base_left_released();

	void on_pushButton_base_left_pressed();

    void on_pushButton_close_gripper_clicked();

    void on_pushButton_open_gripper_clicked();

    void on_pushButton_start_position_arm_clicked();

    void on_pushButton_calibrate_tape_clicked();

    void on_pushButton_calibrate_floor_clicked();

    void on_connect_action_triggered();

    void request_data();

    void send_heartbeat();

    void draw_graphs();

    void on_actionDisconnect_triggered();

    void on_pushButton_send_param_clicked();

    void horzScrollBarChanged(int);

    void on_pushButton_stop_pressed();

    void wheelevent_steering(QWheelEvent*);

    void add_to_lcdtimer();

    void on_pushButton_send_arm_pos_clicked();

    void on_pushButton_pause_graph_clicked();

    void on_request_button_clicked();

    void on_transmit_button_clicked();

	void on_pushButton_y_upp_pressed();

	void on_pushButton_y_down_pressed();

	void on_pushButton_x_up_pressed();

	void on_pushButton_x_down_pressed();

	void on_pushButton_y_upp_released();

	void on_pushButton_y_down_released();

	void on_pushButton_x_up_released();

	void on_pushButton_x_down_released();

	void on_pushButton_pickup_right_clicked();

	void on_pushButton_pickup_left_clicked();

private:
    Ui::MainWindow *ui;

    void set_up_graphs();

    QTimer *timer_heartbeat = new QTimer();
    QTimer *timer_req = new QTimer();
    QTimer *timer_com = new QTimer(); //When timer_com i started for the first time, start_time has to be set to current time.
    QTimer *timer_graph = new QTimer();

    QTime *start_time;

    bluetooth* port = NULL;

    bool update_graph = true;

    QVector<QGraphicsEllipseItem*> linesensor_circels;

	QVector<double> times_mass, value_mass;
	QVector<double> times_steering, value_steering;
    QVector<double> times_range_1, times_range_2, value_range_1, value_range_2;

    QGraphicsScene* linesensor_plot = new QGraphicsScene();

	//XXX: This is not a good soultion, place until better is found
	bool station = false;

	bool validate_spoof();
};

#endif // MAINWINDOW_H
