#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
