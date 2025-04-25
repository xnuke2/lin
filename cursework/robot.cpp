// robot.cpp
#include "robot.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <cmath>
#include <cstring>

const std::string Robot::FIFO_ROBOT = "/tmp/robot_fifo";
const std::string Robot::FIFO_CONTROL = "/tmp/control_fifo";

Robot::Robot() : position{ 0.0, 0.0 }, speed(1.0), battery(100), running(true) {
    mkfifo(FIFO_ROBOT.c_str(), 0666);
    mkfifo(FIFO_CONTROL.c_str(), 0666);

    fd_robot = open(FIFO_ROBOT.c_str(), O_RDONLY | O_NONBLOCK);
    fd_control = open(FIFO_CONTROL.c_str(), O_WRONLY);

    if (fd_robot == -1 || fd_control == -1) {
        throw std::runtime_error("Failed to open FIFO pipes");
    }
}

Robot::~Robot() {
    close(fd_robot);
    close(fd_control);
    unlink(FIFO_ROBOT.c_str());
    unlink(FIFO_CONTROL.c_str());
}

void Robot::run() {
    std::cout << "Робот инициализирован в позиции ("
        << position.x << ", " << position.y << ")\n";

    char buffer[256];
    while (running) {
        ssize_t n = read(fd_robot, buffer, sizeof(buffer));
        if (n > 0) {
            buffer[n] = '\0';
            processCommand(buffer);
        }
        usleep(100000); // 100ms delay
    }
}

void Robot::processCommand(const std::string& cmd) {
    Command command;
    sscanf(cmd.c_str(), "%d %lf %lf %lf %ld",
        reinterpret_cast<int*>(&command.type),
        &command.x, &command.y, &command.speed, &command.timestamp);

    switch (command.type) {
    case CommandType::MOVE:
        moveTo(command.x, command.y);
        break;
    case CommandType::STATUS:
        sendStatus();
        break;
    case CommandType::CHANGE_SPEED:
        changeSpeed(command.speed);
        break;
    case CommandType::EMERGENCY_STOP:
        emergencyStop();
        break;
    }
}

void Robot::moveTo(double x, double y) {
    double distance = sqrt(pow(x - position.x, 2) + pow(y - position.y, 2));
    double time = distance / speed;

    std::cout << "Движение к (" << x << ", " << y << "), время: " << time << " сек\n";
    sleep(static_cast<int>(time));

    position = { x, y };
    lastUpdate = time(nullptr);
    battery -= static_cast<int>(distance);
    sendStatus();
}

void Robot::changeSpeed(double newSpeed) {
    speed = newSpeed;
    std::cout << "Скорость изменена на: " << speed << "\n";
    sendStatus();
}

void Robot::emergencyStop() {
    std::cout << "АВАРИЙНАЯ ОСТАНОВКА!\n";
    running = false;
}

void Robot::sendStatus() {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "POS:%.1f,%.1f SPD:%.1f BAT:%d",
        position.x, position.y, speed, battery);
    write(fd_control, buffer, strlen(buffer) + 1);
}

void Robot::stop() {
    running = false;
}