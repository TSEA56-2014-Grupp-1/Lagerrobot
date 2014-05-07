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

    void print_on_log(QString);

    void new_connection(bluetooth*);

    void connect_to_port(QString);

    void add_steering_data(int);

    void set_RFID(QString);

	void update_linesensor_plot(QByteArray*);

	void pickupstation(QByteArray*);


private slots:
    void keyPressEvent(QKeyEvent *key_pressed);

    void keyReleaseEvent(QKeyEvent *key_relased);

    void on_pushButton_forward_pressed();

    void on_pushButton_back_pressed();

    void on_pushButton_forward_released();

    void on_pushButton_back_released();

    void on_pushButton_left_pressed();

    void on_pushButton_left_released();

    void on_pushButton_right_pressed();

    void on_pushButton_right_released();

    void on_pushButton_start_line_clicked();

    void on_pushButton_stop_line_clicked();

    void on_pushButton_close_gripper_clicked();

    void on_pushButton_open_gripper_clicked();

    void on_pushButton_3_upp_pressed();

    void on_pushButton_3_upp_released();

    void on_pushButton_3_down_pressed();

    void on_pushButton_3_down_released();

    void on_pushButton_2_upp_pressed();

    void on_pushButton_2_upp_released();

    void on_pushButton_2_down_pressed();

    void on_pushButton_2_down_released();

    void on_pushButton_1_upp_pressed();

    void on_pushButton_1_upp_released();

    void on_pushButton_1_down_pressed();

    void on_pushButton_1_down_released();

    void on_pushButton_base_left_pressed();

    void on_pushButton_base_left_released();

    void on_pushButton_base_right_pressed();

    void on_pushButton_base_right_released();

    void on_pushButton_start_position_arm_clicked();

    void on_pushButton_put_down_right_clicked();

    void on_pushButton_put_down_left_clicked();

    void on_pushButton_calibrate_tape_clicked();

    void on_pushButton_calibrate_floor_clicked();

    void on_connect_action_triggered();

    void request_data();

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

private:
    Ui::MainWindow *ui;

    void disable_buttons();
    void enable_buttons();

    void set_up_graphs();
    void draw_graphs();

    QTimer *timer_req = new QTimer();
    QTimer *timer_com = new QTimer(); //When timer_com i started for the first time, start_time has to be set to current time.

    QTime *time_graph = new QTime();
    QTime *start_time;

    bluetooth* port = NULL;

    bool update_graph = true;

    QVector<double> times_steering, value_steering;

    QVector<QGraphicsEllipseItem*> linesensor_circels;

    QGraphicsScene* linesensor_plot = new QGraphicsScene();

	//XXX: This is not a good soultion, place until better is found
	bool station = false;

	bool validate_spoof();
};

#endif // MAINWINDOW_H
