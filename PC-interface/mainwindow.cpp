#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QShortcut>
#include <QKeySequence>
#include <QKeyEvent>
#include <iostream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene_graph_1 = new QGraphicsScene(this);
    scene_graph_2 = new QGraphicsScene(this);
    ui->graphicsView_graph_1->setScene(scene_graph_1);
    ui->graphicsView_graph_2->setScene(scene_graph_2);

    pen_steering = new QPen();
    pen_steering->setColor(Qt::black);
    pen_steering->setWidth(2);

    last_xpos_steering = 0;
    last_ypos_steering = 0;

    QTime *time_steering = new QTime();
    time_steering->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *key_pressed) {
    if (key_pressed->key() == Qt::Key_W) {
        on_pushButton_forward_pressed();
    }
    else if (key_pressed->key() == Qt::Key_S) {
        on_pushButton_back_pressed();
    }
    else if (key_pressed->key() == Qt::Key_A) {
        on_pushButton_left_pressed();
    }
    else if (key_pressed->key() == Qt::Key_D) {
        on_pushButton_base_right_pressed();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *key_released) {
    if (key_released->key() == Qt::Key_W) {
        on_pushButton_forward_released();
    }
    else if (key_released->key() == Qt::Key_S) {
        on_pushButton_back_released();
    }
    else if (key_released->key() == Qt::Key_A) {
        on_pushButton_left_released();
    }
    else if (key_released->key() == Qt::Key_D) {
        on_pushButton_base_right_released();
    }
}

void MainWindow::on_pushButton_forward_pressed()
{
    //Go forward
}


void MainWindow::on_pushButton_back_pressed()
{
    //Go backwards
}

void MainWindow::on_pushButton_forward_released()
{
    //Stop going forward
}

void MainWindow::on_pushButton_back_released()
{
    //Stop going backwards
}

void MainWindow::on_pushButton_left_pressed()
{
    //Start turning left
}

void MainWindow::on_pushButton_left_released()
{
    //Stop turning left
}

void MainWindow::on_pushButton_right_pressed()
{
    //start going right
}

void MainWindow::on_pushButton_right_released()
{
    //Stop going right
}

void MainWindow::on_pushButton_start_line_clicked()
{
    //Start follwing line
}

void MainWindow::on_pushButton_stop_line_clicked()
{
    //Stop follwing line
}

void MainWindow::on_lineEdit_Kd_editingFinished()
{
    //Update Kd with current value
}

void MainWindow::on_lineEdit_Kp_editingFinished()
{
    //Update Kp with current value
}

void MainWindow::on_pushButton_close_gripper_clicked()
{
    //close gripper
}

void MainWindow::on_pushButton_open_gripper_clicked()
{
    //open gripper
}

void MainWindow::on_pushButton_3_upp_pressed()
{
    //Start moving 3 joint upp
}

void MainWindow::on_pushButton_3_upp_released()
{
    //Stop moving 3 joint upp
}

void MainWindow::on_pushButton_3_down_pressed()
{
    //Start moving 3 joint down
}

void MainWindow::on_pushButton_3_down_released()
{
    //Stop moving 3 joint down
}

void MainWindow::on_pushButton_2_upp_pressed()
{
    //Start moving 2 joint upp
}

void MainWindow::on_pushButton_2_upp_released()
{
    //Stop moving 2 joint upp
}

void MainWindow::on_pushButton_2_down_pressed()
{
    //Start moving 2 joint down
}

void MainWindow::on_pushButton_2_down_released()
{
    //Stop moving 2 joint down
}

void MainWindow::on_pushButton_1_upp_pressed()
{
    //Start moving 1 joint upp
}

void MainWindow::on_pushButton_1_upp_released()
{
    //Stop moving 1 joint upp
}

void MainWindow::on_pushButton_1_down_pressed()
{
    //Start moving 1 joint down
}

void MainWindow::on_pushButton_1_down_released()
{
    //Stop moving 1 joint down
}

void MainWindow::on_pushButton_base_left_pressed()
{
    //Start to rotate base left
}

void MainWindow::on_pushButton_base_left_released()
{
    //Stop rotating base left
}

void MainWindow::on_pushButton_base_right_pressed()
{
    //Start to rotate base right
}

void MainWindow::on_pushButton_base_right_released()
{
    //Stop rotating base right
}

void MainWindow::on_pushButton_start_position_arm_clicked()
{
    //Move arm to starting position
}

void MainWindow::on_pushButton_put_down_right_clicked()
{
    //Leave object to the right
}

void MainWindow::on_pushButton_put_down_left_clicked()
{
    //Leave object to the left
}


void MainWindow::on_pushButton_calibrate_tape_clicked()
{
    //Calibrate tape
}

void MainWindow::on_pushButton_calibrate_floor_clicked()
{
    //Calibrate floor
}

void MainWindow::draw_next_point_steering(qreal ypos) {
    qreal time = time_steering->elapsed();
    scene_graph_1->addLine(last_xpos_steering, last_ypos_steering, time, ypos, *pen_steering);
    last_xpos_steering = time;
    last_ypos_steering = ypos;
    qDebug() << last_ypos_steering;
    qDebug() << "Time: " << last_xpos_steering;
}
