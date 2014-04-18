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
#include <qmath.h>
#include "bluetooth.h"
#include "../shared/packets.h"

/*
 *      @brief Constructor for main window
 *
 *      @param parent Parent for the window
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->listWidget_log->setFocusPolicy(Qt::ClickFocus);

    timer->setInterval(250);
    connect(timer, SIGNAL(timeout()), this, SLOT(request_data()));

    disable_buttons();
    ui->actionDisconnect->setEnabled(false);

    set_up_graphs();
    connect(ui->horizontalScrollBar_graphs, SIGNAL(valueChanged(int)), this, SLOT(horzScrollBarChanged(int)));
    connect(ui->plot_steering, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(wheelevent_steering(QWheelEvent*)));
}

//XXX: TODO:
MainWindow::~MainWindow()
{
    delete ui;
}


/*
 *      @brief Linking w, a, s and d to forward, left, back and right buttons key_pressed_event
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
        //on_pushButton_base_left_pressed();
    }
    else if (key_pressed->key() == Qt::Key_D) {
        on_pushButton_right_pressed();
        //on_pushButton_base_right_pressed();
    }
    else if (key_pressed->key() == Qt::Key_Escape) {
        on_pushButton_stop_pressed();
    }
}

/*
 *      @brief Linking w, a, s and d to forward, left, back and right buttons key_released_event
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
        //on_pushButton_base_left_released();
    }
    else if (key_released->key() == Qt::Key_D) {
        on_pushButton_right_released();
        //on_pushButton_base_right_released();
    }
}


/*
 *      @brief Setting new port for program to communicate with robot
 *
 *      @param new_connection Bluetooth port to robot
 */
void MainWindow::new_connection(bluetooth *new_connection) {
    port = new_connection;
}

/*
 *      @brief Function to connect to new port
 *
 *      @param name Name of the port
 */
void MainWindow::connect_to_port(QString name) {
    bluetooth *connection = new bluetooth(name, this);
    print_on_log(QObject::tr("Connecting to port: %1").arg(name));
    new_connection(connection);
    if(port->open_port()) {
        print_on_log("Bluetooth connected succesfully.");
        enable_buttons();
        timer->start();
    }
    else {
        print_on_log("Error opening bluetooth");
        delete port;
        port = NULL;
    }

}

void MainWindow::on_pushButton_forward_pressed()
{
    port->send_packet(PKT_CHASSIS_COMMAND, 2, CMD_CHASSIS_MOVEMENT, 1);
}

void MainWindow::on_pushButton_back_pressed()
{
    port->send_packet(PKT_CHASSIS_COMMAND, 2, CMD_CHASSIS_MOVEMENT, 2);
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
    port->send_packet(PKT_CHASSIS_COMMAND, 2, CMD_CHASSIS_MOVEMENT, 4);
}

void MainWindow::on_pushButton_left_released()
{
    //Stop turning left
}

void MainWindow::on_pushButton_right_pressed()
{
    port->send_packet(PKT_CHASSIS_COMMAND, 2, CMD_CHASSIS_MOVEMENT, 3);
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
 *      @brief Prints a string in the log window, will be timestamped
 *
 *      @param text Text to be displated on in the log window
 */
void MainWindow::print_on_log(QString text) {
    text.prepend(QTime::currentTime().toString("hh:mm:ss").prepend("[").append("] "));
    ui->listWidget_log->addItem(text);
    ui->listWidget_log->scrollToBottom();
}

/*
 *      @brief Open a new Dialog_connect
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
 *      @brief Disable all buttons
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
    ui->pushButton_send_param->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
}

/*
 *      @brief Enable all buttons
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
    ui->pushButton_send_param->setEnabled(true);
    ui->pushButton_stop->setEnabled(true);
}

/*
 *      @brief Function that will reqeuset line data from robot.
 *      @details Function that will request line data from robot, will also reset the timer so that interupt will happen again.
 */
void MainWindow::request_data() {
    port->send_packet(PKT_PACKET_REQUEST, 1, PKT_LINE_DATA);
    timer->start();
}

/*
 *      @brief Callback for diconnect button.
 *      @details Callback for disconnect button, will disconnect and delete the QSerialPort. Will also disable all buttons to aviod calls to nullpointer.
 */
void MainWindow::on_actionDisconnect_triggered()
{
    port->disconnect();
    ui->actionDisconnect->setEnabled(false);
    disable_buttons();
    delete port;
}


/*
 *      @brief Function to send Kp and Kd to robot.
 */
void MainWindow::on_pushButton_send_param_clicked()
{
    uint8_t Kp = ui->lineEdit_Kp->text().toInt();
    uint8_t Kd = ui->lineEdit_Kd->text().toInt();
    port->send_packet(PKT_CHASSIS_COMMAND , 3,CMD_CHASSIS_PARAMETERS, Kp, Kd);
    print_on_log(QString::number(PKT_CHASSIS_COMMAND));
    print_on_log(QString::number(CMD_CHASSIS_PARAMETERS));
}

/*
 *      @brief Setting up graphs with correct settings and legends.
 */
void MainWindow::set_up_graphs() {
    ui->plot_steering->addGraph();
    ui->plot_steering->xAxis->setLabel("Time");
    ui->plot_steering->xAxis->setAutoTickStep(false);
    ui->plot_steering->xAxis->setTickStep(1);
    ui->plot_steering->yAxis->setRange(-127, 127);
}

/*
 *      @brief Add data to steeringvectors, then calls draw_graphs().
 *
 *      @param new_data Data that will be added.
 */
void MainWindow::add_steering_data(int new_data) {
    times.push_back(time->elapsed()/1000);
    value_steering.push_back(new_data);
    draw_graphs();
}

/*
 *      @brief Draw all data on the graphs.
 */
void MainWindow::draw_graphs() {
    ui->plot_steering->graph(0)->setData(times, value_steering);
    ui->plot_steering->xAxis->setRange(time->elapsed()/1000 - 5, time->elapsed()/1000);
    ui->plot_steering->replot();
    ui->horizontalScrollBar_graphs->setRange(0,time->elapsed()/100); //Setting the scrollbar value times 10 to make scrolling smooth
}

/*
 *      @brief Changes the plot when scrollbar is changed.
 *
 *      @param value Value of the scrollbar.
 */
void MainWindow::horzScrollBarChanged(int value) {
    if (qAbs(ui->plot_steering->xAxis->range().center()-value/10) > 0.1) // if user is dragging plot, we don't want to replot twice
    {
        ui->plot_steering->xAxis->setRange(value/10, ui->plot_steering->xAxis->range().size(), Qt::AlignCenter);
        ui->plot_steering->replot();
    }
}

/*
 *      @brief Callback for stop button, will send packet to stop the robot.
 */
void MainWindow::on_pushButton_stop_pressed()
{
    port->send_packet(PKT_CHASSIS_COMMAND, 2, CMD_CHASSIS_MOVEMENT, 0);
}

/*
 *      @brief Set new value to RFID label.
 *
 *      @param new_RFID Value of the new RFID card.
 */
void MainWindow::set_RFID(QString new_RFID) {
    ui->label_RFID->setText(new_RFID);
    ui->label_RFID_time->setText(QTime::currentTime().toString("hh:mm:ss"));
}

void MainWindow::wheelevent_steering(QWheelEvent* event) {
    int steps = qCeil(event->delta());

    steps += ui->horizontalScrollBar_graphs->value();

    ui->horizontalScrollBar_graphs->setValue(steps);
}
