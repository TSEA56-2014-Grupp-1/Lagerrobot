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

    heartbeat_timer->setInterval(1000);
    connect(heartbeat_timer, SIGNAL(timeout()), this, SLOT(send_heartbeat()));

    timer_req->setInterval(125);
    connect(timer_req, SIGNAL(timeout()), this, SLOT(request_data()));

    timer_com->setInterval(500); // robot will emergency stop after ~1.1 seconds, after two missed heartbeats
    connect(timer_com, SIGNAL(timeout()), this, SLOT(add_to_lcdtimer()));
    ui->lcdTimer->setDigitCount(10);

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
    delete port;
    delete timer_req;
    delete timer_com;
    delete start_time;
    delete time_graph;
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
        heartbeat_timer->start();
        timer_req->start();
        time_graph->start();
    }
    else {
        print_on_log("Error opening bluetooth");
        delete port;
        port = NULL;
    }

}

void MainWindow::on_pushButton_forward_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_CHASSIS_COMMAND, 2, CMD_CHASSIS_MOVEMENT, 1);
	}
}

void MainWindow::on_pushButton_back_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_CHASSIS_COMMAND, 2, CMD_CHASSIS_MOVEMENT, 2);
	}
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
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_CHASSIS_COMMAND, 2, CMD_CHASSIS_MOVEMENT, 4);
	}
}

void MainWindow::on_pushButton_left_released()
{
    //Stop turning left
}

void MainWindow::on_pushButton_right_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_CHASSIS_COMMAND, 2, CMD_CHASSIS_MOVEMENT, 3);
	}
}

void MainWindow::on_pushButton_right_released()
{
    //Stop going right
}

void MainWindow::on_pushButton_start_line_clicked()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_CHASSIS_COMMAND, 1, CMD_CHASSIS_START);
		timer_com->start();
		start_time = new QTime(QTime::currentTime());
	}
}

void MainWindow::on_pushButton_stop_line_clicked()
{
    //XXX: Sending packet to stop robot. Maybe should implement another packet to only stop linefollowing?
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_STOP,0);
	}
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
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 1, 4);
	}
}

void MainWindow::on_pushButton_3_upp_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 4);
	}
}

void MainWindow::on_pushButton_3_down_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 0, 4);
	}
}

void MainWindow::on_pushButton_3_down_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 4);
	}
}

void MainWindow::on_pushButton_2_upp_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 1, 3);
	}
}

void MainWindow::on_pushButton_2_upp_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 3);
	}
}

void MainWindow::on_pushButton_2_down_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 0, 3);
	}
}

void MainWindow::on_pushButton_2_down_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 3);
	}
}

void MainWindow::on_pushButton_1_upp_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 1, 2);
	}
}

void MainWindow::on_pushButton_1_upp_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 2);
	}
}

void MainWindow::on_pushButton_1_down_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 0, 2);
	}
}

void MainWindow::on_pushButton_1_down_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 2);
	}
}

void MainWindow::on_pushButton_base_left_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 0, 1);
	}
}

void MainWindow::on_pushButton_base_left_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 1);
	}
}

void MainWindow::on_pushButton_base_right_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 3, CMD_ARM_MOVE, 1, 1);
	}
}

void MainWindow::on_pushButton_base_right_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_STOP, 1);
	}
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
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_CALIBRATION_COMMAND, 2, CAL_LINE, 1);
	}
}

void MainWindow::on_pushButton_calibrate_floor_clicked()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_CALIBRATION_COMMAND, 2, CAL_LINE, 0);
	}
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
    ui->pushButton_send_arm_pos->setEnabled(false);
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
    ui->pushButton_send_arm_pos->setEnabled(true);
}

/*
 *      @brief Function that will reqeuset line data from robot.
 *      @details Function that will request line data from robot, will also reset the timer so that interupt will happen again.
 */
void MainWindow::request_data() {
    if (port == NULL) {
        print_on_log("No port to send to.");
    }
    else {
        //port->send_packet(PKT_PACKET_REQUEST, 1, PKT_LINE_DATA);
        timer_req->start();
    }
}
void MainWindow::send_heartbeat()
{
    if (port != nullptr) {
        port->send_packet(PKT_HEARTBEAT);
        heartbeat_timer->start();
    }
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
    port = NULL;
}


/*
 *      @brief Function to send Kp and Kd to robot.
 */
void MainWindow::on_pushButton_send_param_clicked()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		uint8_t Kp = ui->lineEdit_Kp->text().toInt();
		uint8_t Kd = ui->lineEdit_Kd->text().toInt();
		port->send_packet(PKT_CHASSIS_COMMAND , 3,CMD_CHASSIS_PARAMETERS, Kp, Kd);
		print_on_log(QString::number(PKT_CHASSIS_COMMAND));
		print_on_log(QString::number(CMD_CHASSIS_PARAMETERS));
	}
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
    ui->plot_steering->graph()->setLineStyle(QCPGraph::lsLine);
    ui->plot_steering->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

    ui->graphicsView_linesensor->setScene(linesensor_plot);
    ui->graphicsView_linesensor->show();
    linesensor_circels.resize(11);
    for (int i = 0; i < 11; ++i){
        linesensor_circels[i] = linesensor_plot->addEllipse(30*i,0,13,13);
        linesensor_circels[i]->setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
        linesensor_circels[i]->setOpacity(1);
    }
}

/*
 *      @brief Add data to steeringvectors, then calls draw_graphs().
 *
 *      @param new_data Data that will be added.
 */
void MainWindow::add_steering_data(int new_data) {
    times_steering.push_back((double)time_graph->elapsed()/1000);
    value_steering.push_back((quint8)new_data - 127);
    if (update_graph) { //XXX: Part of bad solution, see on_pushButton_pause_graph()
        draw_graphs();
    }
}

/*
 *      @brief Draw all data on the graphs.
 */
void MainWindow::draw_graphs() {
    ui->plot_steering->graph(0)->setData(times_steering, value_steering);
    ui->plot_steering->xAxis->setRange((double)(time_graph->elapsed()/1000) - 10,
                                       (double)time_graph->elapsed()/1000);
    ui->plot_steering->replot();
    ui->horizontalScrollBar_graphs->setRange(0,(double)time_graph->elapsed()/100); //Setting the scrollbar value times 10 to make scrolling smooth
    ui->horizontalScrollBar_graphs->setValue(time_graph->elapsed()/100);
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
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_CHASSIS_COMMAND, 2, CMD_CHASSIS_MOVEMENT, 0);
	}
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

/*
 *      @brief Slot for scrolling with trackpad.
 *
 *      @param event The QWheelEvent that triggerd the signal.
 */
void MainWindow::wheelevent_steering(QWheelEvent* event) {
    int steps = qCeil(event->delta());

    steps += ui->horizontalScrollBar_graphs->value();

    ui->horizontalScrollBar_graphs->setValue(steps);
}

/*
 *      @brief Subtracts to QTimes.
 *      @details Subtracts to QTimes, first has to be before second. Can only handle minutes, seconds and mseconds.
 *
 *      @param first First QTime, has to be before second.
 *      @param second Second QTime, has to be after first.
 *
 *      @return A QTime with the diffrene of first and second. If second was before first it will return the time 0.
 */
inline QTime qtime_subtraction(const QTime& first, const QTime& second) {
    int min = 0;
    int sec = 0;
    int msec = 0;

    msec = first.msec() - second.msec();

    if (msec < 0) {
        msec += 1000;
        sec = -1;
    }

    sec += first.second() - second.second();

    if (sec < 0) {
        sec += 60;
        min = -1;
    }
    else if (sec/60 > 1) {
        sec -= 60;
        min = 1;
    }

    min += first.minute() - second.minute();

    if (min < 0) {
        min += 60;
    }
    else if (min/60 > 1) {
        min -= 60;
    }

    if (min < 0 || sec < 0 || msec < 0)
        return QTime(0,0);
    else
        return QTime(0, min, sec, msec);
}

/*
 *      @brief Update the number on the LCD with the time since start.
 */
void MainWindow::add_to_lcdtimer() {
    QTime time_to_display = qtime_subtraction(QTime::currentTime(), *start_time);
    ui->lcdTimer->display(time_to_display.toString("mm:ss"));
    timer_com->start();
}

/*
 *      @brief Update the plot with linesensors, setting the darknes between 0 and 255.
 *
 *      @param sensor_values QByteArray with the sensor values.
 */
void MainWindow::update_linesensor_plot(QByteArray* sensor_values) {
    QBrush* brush = new QBrush(Qt::SolidPattern);
    for (int i = 0; i < 11; ++i) {
        brush->setColor(QColor(0,0,0,(quint8)sensor_values->at(i)));
        linesensor_circels[i]->setBrush(*brush);
    }
    delete brush;
}

/*
 *      @brief Send position to arm.
 */
void MainWindow::on_pushButton_send_arm_pos_clicked()
{
    bool x_check;
    bool y_check;
    bool angle_check;
	quint16 x = ui->lineEdit_x_cord->text().toInt(&x_check);
	quint16 y = ui->lineEdit_y_cord->text().toInt(&y_check);
	quint16 angle = ui->lineEdit_angle->text().toInt(&angle_check);
    if (port == NULL) {
        print_on_log("No port to send to.");
    }
    if (x_check && y_check && angle_check){
		port->send_packet(PKT_ARM_COMMAND, 6, CMD_ARM_MOVE_POS,
						  (quint8)(x >> 8), (quint8)x, (quint8)(y >> 8),
						  (quint8)y, angle);
    }
    else
        print_on_log("Invalid arguments to arm position, must be integers.");
}

//XXX: This is not a good solution... Inplace until better is found
void MainWindow::on_pushButton_pause_graph_clicked()
{
    if (update_graph) {
        ui->pushButton_pause_graph->setText("Unpause graph");
    }
    else {
        ui->pushButton_pause_graph->setText("Pause graph");
    }
    update_graph = !update_graph;
}
