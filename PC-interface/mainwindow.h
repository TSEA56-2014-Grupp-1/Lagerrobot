#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTimer>
#include <QTime>
#include <QtCore/qmath.h>
#include <QShortcut>
#include <QKeySequence>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void draw_next_point_steering(qreal);

    QTime *time = new QTime();


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

    void on_lineEdit_Kd_editingFinished();

    void on_lineEdit_Kp_editingFinished();

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

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene_graph_steering;
    QGraphicsScene *scene_graph_sensors;

    QPen *pen_steering;

    qreal last_xpos_steering;
    qreal last_ypos_steering;

    void draw_x_axis(QGraphicsScene*);
    void draw_y_axis_steering();
    void draw_y_axis_sensors();


    //XXX: Should not be const, should be upadte every time we get a new highest value
    const int MAX_Y_STEERING = 200;
    const int MAX_Y_SENSORS = 200;

    //Constants for painting graphs
    const int X_SCALE_STEERING = 10;
    const int Y_SCALE_STEERING = 10;

    const int X_SCALE_SENSORS = 10;
    const int Y_SCALE_SENSORS = 10;

    const int Y_INTERVAL_STEERING = 1000;
    const int Y_INTERVAL_SENSOR = 50;
};

#endif // MAINWINDOW_H
