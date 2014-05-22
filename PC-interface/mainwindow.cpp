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

    timer_com->setInterval(100);
    connect(timer_com, SIGNAL(timeout()), this, SLOT(add_to_lcdtimer()));
    ui->lcdTimer->setDigitCount(10);

    timer_graph->setInterval(30);
    connect(timer_graph, SIGNAL(timeout()), this, SLOT(draw_graphs()));

    disable_buttons();
    ui->actionDisconnect->setEnabled(false);

    set_up_graphs();
    connect(ui->horizontalScrollBar_graphs, SIGNAL(valueChanged(int)), this, SLOT(horzScrollBarChanged(int)));
    connect(ui->plot_steering, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(wheelevent_steering(QWheelEvent*)));
	connect(ui->plot_sensor, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(wheelevent_steering(QWheelEvent*)));
}

//XXX: TODO:
MainWindow::~MainWindow()
{
    delete ui;
//    delete port;
//    delete timer_com;
//    delete start_time;
//    delete time_graph;
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
    }
    else if (key_pressed->key() == Qt::Key_D) {
        on_pushButton_right_pressed();
    }
    else if (key_pressed->key() == Qt::Key_Escape) {
        on_pushButton_stop_pressed();
    }
	else if (key_pressed->key() == Qt::Key_Z) {
		on_pushButton_base_left_pressed();
	}
	else if (key_pressed->key() == Qt::Key_X) {
		on_pushButton_base_right_pressed();
	}
	else if (key_pressed->key() == Qt::Key_Up) {
		on_pushButton_y_upp_pressed();
	}
	else if (key_pressed->key() == Qt::Key_Down) {
		on_pushButton_y_down_pressed();
	}
	else if (key_pressed->key() == Qt::Key_Left) {
		on_pushButton_x_down_pressed();
	}
	else if (key_pressed->key() == Qt::Key_Right) {
		on_pushButton_x_up_pressed();
	}
}

void MainWindow::keyReleaseEvent(QKeyEvent *key_released) {
	if (key_released->key() == Qt::Key_Z) {
		on_pushButton_base_left_released();
	}
	else if (key_released->key() == Qt::Key_X) {
		on_pushButton_base_right_released();
	}
	else if (key_released->key() == Qt::Key_Up) {
		on_pushButton_y_upp_released();
	}
	else if (key_released->key() == Qt::Key_Down) {
		on_pushButton_y_down_released();
	}
	else if (key_released->key() == Qt::Key_Left) {
		on_pushButton_x_down_released();
	}
	else if (key_released->key() == Qt::Key_Right) {
		on_pushButton_x_up_released();
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

        times_mass.clear();
        times_steering.clear();
        times_range_1.clear();
        times_range_2.clear();

        value_mass.clear();
        value_steering.clear();
        value_range_1.clear();
        value_range_2.clear();
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


void MainWindow::on_pushButton_left_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_CHASSIS_COMMAND, 2, CMD_CHASSIS_MOVEMENT, 4);
	}
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

void MainWindow::on_pushButton_start_line_clicked()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_CHASSIS_COMMAND, 1, CMD_CHASSIS_START);
        times_mass.clear();
        times_steering.clear();
        times_range_1.clear();
        times_range_2.clear();

        value_mass.clear();
        value_steering.clear();
        value_range_1.clear();
        value_range_2.clear();

        time_graph->start();
		timer_com->start();
		start_time = new QTime(QTime::currentTime());
	}
}

void MainWindow::on_pushButton_stop_line_clicked()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_CHASSIS_COMMAND, 1, CMD_CHASSIS_STOP);
	}
}

void MainWindow::on_pushButton_close_gripper_clicked()
{
	port->send_packet(PKT_ARM_COMMAND, 1, CMD_ARM_GRIP);
}

void MainWindow::on_pushButton_open_gripper_clicked()
{
	port->send_packet(PKT_ARM_COMMAND, 1, CMD_ARM_RELEASE);
}

void MainWindow::on_pushButton_base_left_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 2, 1, 1);
	}
}

void MainWindow::on_pushButton_base_left_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 2, 1, 0);
	}
}

void MainWindow::on_pushButton_base_right_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 2, 0, 1);
	}
}

void MainWindow::on_pushButton_base_right_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 2, 0, 0);
	}
}

void MainWindow::on_pushButton_start_position_arm_clicked()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_PREDEFINED_POS, 1);
	}
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
	//XXX:
	station = false;
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
    ui->pushButton_base_left->setEnabled(false);
    ui->pushButton_base_right->setEnabled(false);
    ui->pushButton_calibrate_floor->setEnabled(false);
    ui->pushButton_calibrate_tape->setEnabled(false);
    ui->pushButton_close_gripper->setEnabled(false);
	ui->pushButton_forward->setEnabled(false);
    ui->pushButton_left->setEnabled(false);
    ui->pushButton_open_gripper->setEnabled(false);
	ui->pushButton_pickup_left->setEnabled(false);
	ui->pushButton_pickup_right->setEnabled(false);
    ui->pushButton_right->setEnabled(false);
    ui->pushButton_start_line->setEnabled(false);
    ui->pushButton_start_position_arm->setEnabled(false);
    ui->pushButton_stop_line->setEnabled(false);
    ui->pushButton_back->setEnabled(false);
    ui->pushButton_send_param->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
    ui->pushButton_send_arm_pos->setEnabled(false);
    ui->pushButton_x_down->setEnabled(false);
    ui->pushButton_x_up->setEnabled(false);
    ui->pushButton_y_down->setEnabled(false);
    ui->pushButton_y_upp->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
    ui->transmit_button->setEnabled(false);
    ui->request_button->setEnabled(false);
}

/*
 *      @brief Enable all buttons
 */
void MainWindow::enable_buttons() {
    ui->pushButton_base_left->setEnabled(true);
    ui->pushButton_base_right->setEnabled(true);
    ui->pushButton_calibrate_floor->setEnabled(true);
    ui->pushButton_calibrate_tape->setEnabled(true);
    ui->pushButton_close_gripper->setEnabled(true);
    ui->pushButton_forward->setEnabled(true);
    ui->pushButton_left->setEnabled(true);
    ui->pushButton_open_gripper->setEnabled(true);
	ui->pushButton_pickup_left->setEnabled(true);
	ui->pushButton_pickup_right->setEnabled(true);
    ui->pushButton_right->setEnabled(true);
    ui->pushButton_start_line->setEnabled(true);
    ui->pushButton_start_position_arm->setEnabled(true);
    ui->pushButton_stop_line->setEnabled(true);
    ui->pushButton_back->setEnabled(true);
    ui->pushButton_send_param->setEnabled(true);
    ui->pushButton_stop->setEnabled(true);
    ui->pushButton_send_arm_pos->setEnabled(true);
    ui->pushButton_x_down->setEnabled(true);
    ui->pushButton_x_up->setEnabled(true);
    ui->pushButton_y_down->setEnabled(true);
    ui->pushButton_y_upp->setEnabled(true);
    ui->pushButton_stop->setEnabled(true);
    ui->transmit_button->setEnabled(true);
    ui->request_button->setEnabled(true);
}

/*
 *      @brief Callback for diconnect button.
 *      @details Callback for disconnect button, will disconnect and delete the QSerialPort. Will also disable all buttons to aviod calls to nullpointer.
 */
void MainWindow::on_actionDisconnect_triggered()
{
    port->disconnect();
    ui->actionDisconnect->setEnabled(false);
    ui->connect_action->setEnabled(true);
    disable_buttons();

    timer_graph->stop();
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
	}
}

/*
 *      @brief Setting up graphs with correct settings and legends.
 */
void MainWindow::set_up_graphs() {
    ui->plot_steering->addGraph();
	ui->plot_steering->addGraph();
    ui->plot_steering->xAxis->setLabel("Time");
    ui->plot_steering->xAxis->setAutoTickStep(false);
    ui->plot_steering->xAxis->setTickStep(1);
	ui->plot_steering->yAxis->setRange(-200, 200);

    ui->plot_steering->graph(0)->setLineStyle(QCPGraph::lsLine);
	ui->plot_steering->graph(0)->setName("Center of mass");

    ui->plot_steering->graph(1)->setLineStyle(QCPGraph::lsLine);
	ui->plot_steering->graph(1)->setPen(QPen(QColor(204,0,0)));
	ui->plot_steering->graph(1)->setName("Steering");

	QFont legendFont = font();
	legendFont.setPointSize(11);
	ui->plot_steering->legend->setFont(legendFont);
    ui->plot_steering->legend->setVisible(true);
    ui->plot_steering->axisRect()->insetLayout()->addElement(ui->plot_steering->legend, Qt::AlignLeft);

	//XXX: Set up graph for sensor (plot_sensor)
	ui->plot_sensor->addGraph();
    ui->plot_sensor->addGraph();

    ui->plot_sensor->xAxis->setLabel("Time");
	ui->plot_sensor->xAxis->setAutoTickStep(false);
	ui->plot_sensor->xAxis->setTickStep(1);

    ui->plot_sensor->yAxis->setRange(0,335);
    ui->plot_sensor->yAxis->setLabel("Distance [mm]");
    ui->plot_sensor->yAxis->setAutoTickStep(false);
    ui->plot_sensor->yAxis->setTickStep(100);

    ui->plot_sensor->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot_sensor->graph(0)->setName("Left sensor");

    ui->plot_sensor->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->plot_sensor->graph(1)->setPen(QPen(QColor(204,0,0)));
    ui->plot_sensor->graph(1)->setName("Right sensor");

    ui->plot_sensor->legend->setFont(legendFont);
    ui->plot_sensor->legend->setVisible(true);
    ui->plot_sensor->axisRect()->insetLayout()->addElement(ui->plot_sensor->legend, Qt::AlignLeft);


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
void MainWindow::add_mass_data(int new_data) {
	times_mass.push_back((double)time_graph->elapsed()/1000);
	value_mass.push_back((quint8)new_data - 127);
	draw_graphs();
}

void MainWindow::add_steering_data(int new_data) {
    times_steering.push_back((double)time_graph->elapsed()/1000);
    value_steering.push_back((double) new_data / 10);
	draw_graphs();
}

void MainWindow::add_range_data(quint8 sensor, quint16 new_data) {
    if (sensor == 0) {
        times_range_1.push_back((double)time_graph->elapsed()/1000);
        value_range_1.push_back((double)new_data);
    } else if (sensor == 1) {
        times_range_2.push_back((double)time_graph->elapsed()/1000);
        value_range_2.push_back((double)new_data);
    }
    draw_graphs();
}

/*
 *      @brief Draw all data on the graphs.
 */
void MainWindow::draw_graphs() {
	if (!update_graph) { //XXX: Part of bad solution, see on_pushButton_pause_graph()
		return;
	}
    timer_graph->start();

	ui->plot_steering->graph(0)->setData(times_mass, value_mass);
	ui->plot_steering->graph(1)->setData(times_steering, value_steering);

    ui->plot_steering->xAxis->setRange(((double)time_graph->elapsed()/1000) - 10,
                                       (double)time_graph->elapsed()/1000);

    ui->plot_sensor->xAxis->setRange(((double)time_graph->elapsed()/1000) - 10,
                                     (double)time_graph->elapsed()/1000);

    ui->plot_sensor->graph(0)->setData(times_range_1, value_range_1);
    ui->plot_sensor->graph(1)->setData(times_range_2, value_range_2);
    ui->plot_steering->replot();
	ui->plot_sensor->replot();
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
        ui->plot_steering->xAxis->setRange((double)value/10, ui->plot_steering->xAxis->range().size(), Qt::AlignRight);
        ui->plot_sensor->xAxis->setRange((double)value/10, ui->plot_sensor->xAxis->range().size(), Qt::AlignRight);
        ui->plot_steering->replot();
        ui->plot_sensor->replot();
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
void MainWindow::update_linesensor_plot(QByteArray* parameters) {
    QBrush* brush = new QBrush(Qt::SolidPattern);
	int mask = 1;
	quint16 sensor_values = ((quint16)parameters->at(0) << 8) | (quint16)parameters->at(1);
	brush->setColor(QColor(0,0,0,255));
	for (int i = 0; i < 11; ++i) {
		if (sensor_values & mask) {
			linesensor_circels[i]->setBrush(*brush);
		}
		mask = mask << 1;
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
	qint16 angle = ui->lineEdit_angle->text().toInt(&angle_check);
	float angle_rad = angle*M_PI/180;
	int16_t angle_to_send = angle_rad*500;
    if (port == NULL) {
        print_on_log("No port to send to.");
        return;
    }
    if (x_check && y_check && angle_check){
		port->send_packet(PKT_ARM_COMMAND, 7, CMD_ARM_MOVE_POS,
						  (quint8)(x >> 8), (quint8)x, (quint8)(y >> 8),
						  (quint8)y, (quint8)(angle_to_send >> 8),(quint8)(angle_to_send));
		print_on_log("Sent arm command");
		print_on_log(QObject::tr("x: %1, y: %2, angle: %3").arg(QString::number(x)).arg(QString::number(y)).arg(QString::number(angle_to_send)));
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

/*
 *		0 station vänster
 *		1 ingen station
 *		2 station höger
 */
void MainWindow::pickupstation(QByteArray* data) {
	if (data->at(11) == 2 && !station) {
		print_on_log(QObject::tr("Station left: %1").arg(QString::number(data->at(11))));
		for (int i = 0; i < 11; ++i) {
			print_on_log(QObject::tr("Value of %1: %2").arg(i).arg(QString::number((quint8)data->at(i))));
		}
		station = true;
	}
	else if (data->at(11) == 0 && !station) {
		print_on_log("Station right");
//		update_graph = false;
		for (int i = 0; i < 11; ++i) {
			print_on_log(QObject::tr("Value of %1: %2").arg(i).arg(QString::number((quint8)data->at(i))));
		}
		station = true;
	}
	else if (data->at(11) == 3) {
		print_on_log("Break in line.");
	}
}

bool MainWindow::validate_spoof() {

    if (ui->callback_lineEdit->text().isEmpty() ||
        ui->highByte_lineEdit->text().isEmpty() ||
        ui->lowByte_lineEdit->text().isEmpty() ||
        ui->address_lineEdit->text().isEmpty()) {
        print_on_log("Not enough information entered.");
        return false;
    }
    QRegularExpression re("^[0-9]+$");
    if (!re.match(ui->callback_lineEdit->text()).hasMatch() ||
        !re.match(ui->highByte_lineEdit->text()).hasMatch() ||
        !re.match(ui->lowByte_lineEdit->text()).hasMatch() ||
        !re.match(ui->address_lineEdit->text()).hasMatch()) {
        print_on_log("Only decimal integers accepted");
        return false;
    }
    return true;
}

void MainWindow::on_request_button_clicked() {
    if (!validate_spoof())
        return;
    else {
        if (port == nullptr) {
            print_on_log("No port to send to.");
            return;
        }

        port->send_packet(PKT_SPOOFED_REQUEST,
                          4,
                          ui->address_lineEdit->text().toInt(),
                          ui->callback_lineEdit->text().toInt(),
                          ui->highByte_lineEdit->text().toInt(),
                          ui->lowByte_lineEdit->text().toInt());
    }
}

void MainWindow::on_transmit_button_clicked()
{
    if (!validate_spoof())
        return;
    else {
        if (port == nullptr) {
            print_on_log("No port to send to.");
            return;
        }

        port->send_packet(PKT_SPOOFED_TRANSMIT,
                          4,
                          ui->address_lineEdit->text().toInt(),
                          ui->callback_lineEdit->text().toInt(),
                          ui->highByte_lineEdit->text().toInt(),
                          ui->lowByte_lineEdit->text().toInt());
    }
}

void MainWindow::handle_decision(quint8 decision) {
	if (decision == DEC_ARM_PICKED_UP) {
		print_on_log("Arm has picked up an object");
	}
	else if (decision == DEC_ARM_PUT_DOWN) {
		print_on_log("Arm has put down an object");
	}
	else if (decision == DEC_NO_ID_FOUND) {
		print_on_log("No RFID found");
	}
	else if (decision == DEC_NO_MATCH) {
		print_on_log("RFID did not match");
	}
	else if (decision == DEC_OBJECT_NOT_FOUND) {
		print_on_log("Object was found by arm");
	}
	else if (decision == DEC_PICKUP_LEFT) {
		print_on_log("Picking up object to the left");
	}
	else if (decision == DEC_PICKUP_RIGHT) {
		print_on_log("Picking up object to the right");
	}
	else if (decision == DEC_PUT_DOWN_LEFT) {
		print_on_log("Putting down object to the left");
	}
	else if (decision == DEC_PUT_DOWN_RIGHT) {
		print_on_log("Puttin down object to the right");
	}
	else if (decision == DEC_STATION_HANDELED) {
		print_on_log("Station already handled");
	}
	else if (decision == DEC_UNKOWN_ERROR) {
		print_on_log("Unkown error in chassi");
	}
	else if (decision == DEC_START_LINE) {
		print_on_log("Started linefollowing");
		time_graph->start();
	}
}

void MainWindow::on_pushButton_y_upp_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 1, 1, 1);
	}
}

void MainWindow::on_pushButton_y_down_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 1, 0, 1);
	}
}

void MainWindow::on_pushButton_x_up_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 0, 1, 1);
	}
}

void MainWindow::on_pushButton_x_down_pressed()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 0, 0, 1);
	}
}

void MainWindow::on_pushButton_y_upp_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 1, 1, 0);
	}
}

void MainWindow::on_pushButton_y_down_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 1, 0, 0);
	}
}

void MainWindow::on_pushButton_x_up_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 0, 1, 0);
	}
}

void MainWindow::on_pushButton_x_down_released()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 4, CMD_ARM_MOVE, 0, 0, 0);
	}
}

void MainWindow::on_pushButton_pickup_right_clicked()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_PREDEFINED_POS, 3);
	}
}

void MainWindow::on_pushButton_pickup_left_clicked()
{
	if (port == NULL) {
		print_on_log("No port to send to.");
	}
	else {
		port->send_packet(PKT_ARM_COMMAND, 2, CMD_ARM_PREDEFINED_POS, 2);
	}
}
