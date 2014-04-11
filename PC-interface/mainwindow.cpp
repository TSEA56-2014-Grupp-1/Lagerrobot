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
#include <dialog_connect.h>
#include "bluetooth.h"
#include "../shared/packets.h"

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

    timer->setInterval(250);
    connect(timer, SIGNAL(timeout()), this, SLOT(request_data()));

    disable_buttons();
    ui->actionDisconnect->setEnabled(false);
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
        //on_pushButton_left_pressed();
        on_pushButton_base_left_pressed();
    }
    else if (key_pressed->key() == Qt::Key_D) {
        //on_pushButton_base_right_pressed();
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
        //on_pushButton_left_released();
        on_pushButton_base_left_released();
    }
    else if (key_released->key() == Qt::Key_D) {
        //on_pushButton_base_right_released();
        on_pushButton_base_right_released();
    }
}


/*
 *      Setting new port for program to communicate with robot
 *
 *      @param new_connection Bluetooth port to robot
 */
void MainWindow::new_connection(bluetooth *new_connection) {
    port = new_connection;
}

void MainWindow::connect_to_port(QString name) {
    bluetooth *connection = new bluetooth(name, this);
    print_on_log(QObject::tr("Connecting to port: %1").arg(name));
    new_connection(connection);
    if(port->open_port()) {
        enable_buttons();
        timer->start();
    }
    else {
        delete port;
        port = NULL;
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
    int Kp = ui->lineEdit_Kp->text().toInt();
    int Kd = ui->lineEdit_Kd->text().toInt();
    port->send_packet(CMD_CHASSIS_PARAMETERS, 2, Kp, Kd);
}

void MainWindow::on_lineEdit_Kp_editingFinished()
{
    int Kp = ui->lineEdit_Kp->text().toInt();
    int Kd = ui->lineEdit_Kd->text().toInt();
    port->send_packet(CMD_CHASSIS_PARAMETERS, 2, Kp, Kd);
}

void MainWindow::on_pushButton_close_gripper_clicked()
{
     //port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 1, 6);
}

void MainWindow::on_pushButton_open_gripper_clicked()
{
    //port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 0, 6);
}

void MainWindow::on_pushButton_3_upp_pressed()
{
    port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 1, 4);
}

void MainWindow::on_pushButton_3_upp_released()
{
    port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 4);
}

void MainWindow::on_pushButton_3_down_pressed()
{
    port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 0, 4);
}

void MainWindow::on_pushButton_3_down_released()
{
    port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 4);
}

void MainWindow::on_pushButton_2_upp_pressed()
{
    port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 1, 3);
}

void MainWindow::on_pushButton_2_upp_released()
{
    port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 3);
}

void MainWindow::on_pushButton_2_down_pressed()
{
    port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 0, 3);
}

void MainWindow::on_pushButton_2_down_released()
{
    port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 3);
}

void MainWindow::on_pushButton_1_upp_pressed()
{
    port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 1, 2);
}

void MainWindow::on_pushButton_1_upp_released()
{
    port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 2);
}

void MainWindow::on_pushButton_1_down_pressed()
{
    port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 0, 2);
}

void MainWindow::on_pushButton_1_down_released()
{
    port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 2);
}

void MainWindow::on_pushButton_base_left_pressed()
{
    port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 0, 1);
}

void MainWindow::on_pushButton_base_left_released()
{
    port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 1);
}

void MainWindow::on_pushButton_base_right_pressed()
{
    port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 1, 1);
}

void MainWindow::on_pushButton_base_right_released()
{
    port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 1);
    print_on_log("base right released");
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
    port->send_packet(PKT_CALIBRATION_COMMAND, 2, CAL_LINE, 1);
}

void MainWindow::on_pushButton_calibrate_floor_clicked()
{
    port->send_packet(PKT_CALIBRATION_COMMAND, 2, CAL_LINE, 0);
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
    ui->graphicsView_graph_1->centerOn(current_time, -127/Y_SCALE_STEERING);
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

/*
 *      Prints a string in the log window, will be timestamped
 *
 *      @param text Text to be displated on in the log window
 */
void MainWindow::print_on_log(QString text) {
    text.prepend(QTime::currentTime().toString("hh:mm:ss").prepend("[").append("] "));
    ui->listWidget_log->addItem(text);
    ui->listWidget_log->scrollToBottom();
}

/*
 *      Open a new Dialog_connect
 */
void MainWindow::on_connect_action_triggered()
{
    Dialog_connect* connect_window = new Dialog_connect(this);
    connect_window->exec();
    delete connect_window;
    if (port != NULL) {
        ui->connect_action->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
    }
}

/*
 *      Disable all buttons
 */
void MainWindow::disable_buttons() {
    ui->pushButton_1_down->setEnabled(false);
    ui->pushButton_1_upp->setEnabled(false);
    ui->pushButton_2_down->setEnabled(false);
    ui->pushButton_2_upp->setEnabled(false);
    ui->pushButton_3_down->setEnabled(false);
    ui->pushButton_3_upp->setEnabled(false);
    ui->pushButton_base_left->setEnabled(false);
    ui->pushButton_base_right->setEnabled(false);
    ui->pushButton_calibrate_floor->setEnabled(false);
    ui->pushButton_calibrate_tape->setEnabled(false);
    ui->pushButton_close_gripper->setEnabled(false);
    ui->pushButton_forward->setEnabled(false);
    ui->pushButton_left->setEnabled(false);
    ui->pushButton_open_gripper->setEnabled(false);
    ui->pushButton_put_down_left->setEnabled(false);
    ui->pushButton_put_down_right->setEnabled(false);
    ui->pushButton_right->setEnabled(false);
    ui->pushButton_start_line->setEnabled(false);
    ui->pushButton_start_position_arm->setEnabled(false);
    ui->pushButton_stop_line->setEnabled(false);
    ui->pushButton_back->setEnabled(false);
}

/*
 *      Enable all buttons
 */
void MainWindow::enable_buttons() {
    ui->pushButton_1_down->setEnabled(true);
    ui->pushButton_1_upp->setEnabled(true);
    ui->pushButton_2_down->setEnabled(true);
    ui->pushButton_2_upp->setEnabled(true);
    ui->pushButton_3_down->setEnabled(true);
    ui->pushButton_3_upp->setEnabled(true);
    ui->pushButton_base_left->setEnabled(true);
    ui->pushButton_base_right->setEnabled(true);
    ui->pushButton_calibrate_floor->setEnabled(true);
    ui->pushButton_calibrate_tape->setEnabled(true);
    ui->pushButton_close_gripper->setEnabled(true);
    ui->pushButton_forward->setEnabled(true);
    ui->pushButton_left->setEnabled(true);
    ui->pushButton_open_gripper->setEnabled(true);
    ui->pushButton_put_down_left->setEnabled(true);
    ui->pushButton_put_down_right->setEnabled(true);
    ui->pushButton_right->setEnabled(true);
    ui->pushButton_start_line->setEnabled(true);
    ui->pushButton_start_position_arm->setEnabled(true);
    ui->pushButton_stop_line->setEnabled(true);
    ui->pushButton_back->setEnabled(true);
}

void MainWindow::request_data() {
    //port->send_packet(PKT_PACKET_REQUEST, 1, PKT_LINE_DATA);
    timer->start();
}

void MainWindow::on_actionDisconnect_triggered()
{
    port->disconnect();
    ui->actionDisconnect->setEnabled(false);
    delete port;
}
