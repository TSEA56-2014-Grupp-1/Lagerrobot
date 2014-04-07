/*
 *      Class functions for MainWindow class
 *
 *      @author Patrik Nyberg
 *
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qDebug>
#include <QtCore/qmath.h>


/*
 *      Constructor for main window
 *
 *      @param parent Parent for the window
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene_graph_steering = new QGraphicsScene(this);
    scene_graph_sensors = new QGraphicsScene(this);
    ui->graphicsView_graph_1->setScene(scene_graph_steering);
    ui->graphicsView_graph_2->setScene(scene_graph_sensors);

    pen_steering = new QPen();
    pen_steering->setColor(Qt::black);
    pen_steering->setWidth(2);

    last_xpos_steering = 0;
    last_ypos_steering = 0;

    ui->listWidget_log->setFocusPolicy(Qt::ClickFocus);
}

//XXX: TODO:
MainWindow::~MainWindow()
{
    delete ui;
}


/*
 *      Linking w, a, s and d to forward, left, back and right buttons key_pressed_event
 *
 *      @param key_pressed Key that was currently pressed
 */
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

/*
 *      Linking w, a, s and d to forward, left, back and right buttons key_released_event
 *
 *      @param key_pressed Key that was currently released
 */
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

/*
 *      Draws next point to steering graph, 1 pixel on y-axis i Y_SCALE_STEERING ms
 *
 *      @param ypos Y-position of next data point
 */
void MainWindow::draw_next_point_steering(qreal ypos) {
    qreal current_time = time->elapsed()/X_SCALE_STEERING;
    scene_graph_steering->addLine(last_xpos_steering, -1*last_ypos_steering, current_time, -1*ypos/Y_SCALE_STEERING, *pen_steering);
    last_xpos_steering = current_time;
    last_ypos_steering = ypos/Y_SCALE_STEERING;
    if (ypos > max_y_steering)
        max_y_steering = ypos + Y_INTERVAL_STEERING; //Adding y_interval to ypos, will give better axis.
    draw_x_axis(scene_graph_steering);
    draw_y_axis_steering();
}

/*
 *      Paints x-axis from 0 to current time adding numbers every second
 *
 *      @param graph Graph that the axis should be painted in
 */
void MainWindow::draw_x_axis(QGraphicsScene *graph) {
    graph->addLine(0,0,time->elapsed()/X_SCALE_STEERING,0);

    QGraphicsTextItem* current_number = new QGraphicsTextItem();

    // XXX: There could be a beter soultion since this will rewrite all the numbers every time.
    for (int i = 1000/X_SCALE_STEERING; i < qCeil(time->elapsed()/X_SCALE_STEERING) ; i = i + 1000/X_SCALE_STEERING) {
        current_number = graph->addText(QString::number(i/(1000/X_SCALE_STEERING)));
        current_number->setPos(i-7,-5);
        graph->addLine(i,-2,i,2);
    }
}

/*
 *      Paints y-axis on the steering graph
 */
void MainWindow::draw_y_axis_steering() {
    scene_graph_steering->addLine(0,-1*max_y_steering,0,max_y_steering);

    QGraphicsTextItem* current_number = new QGraphicsTextItem();

    for (int i = -1*qFloor(max_y_steering); i < qCeil(max_y_steering); i = i + Y_INTERVAL_STEERING/Y_SCALE_STEERING) {
        if (i != 0) {
            if (i > 0)
                current_number = scene_graph_steering->addText(QString::number(i*Y_SCALE_STEERING));
            else
                current_number = scene_graph_steering->addText(QString::number(-1*i*Y_SCALE_STEERING));
            current_number->setPos(3,i-15);
            scene_graph_steering->addLine(-2,i,2,i);
        }
    }

}

void MainWindow::print_on_log(QString text) {
    text.prepend(QTime::currentTime().toString("hh:mm:ss").prepend("[").append("] "));
    ui->listWidget_log->addItem(text);
    ui->listWidget_log->scrollToBottom();
}
