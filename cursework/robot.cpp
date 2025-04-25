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


    fd_robot = open(FIFO_ROBOT.c_str(), O_RDONLY);
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
    std::cout << "Робот инициализирован в ("
        << position.x << ", " << position.y << ")\n";

    char buffer[256];
    while (running) {
        ssize_t n = read(fd_robot, buffer, sizeof(buffer));
        if (n > 0) {
            buffer[n] = '\0';
            processCommand(buffer);
        }
        
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
    case CommandType::CHANGE_SPEED:
        changeSpeed(command.speed);
        break;
    case CommandType::EMERGENCY_STOP:
        emergencyStop();
        break;
    case CommandType::STOP:
        stop();
        break;
    }
    sendStatus();
}

void Robot::moveTo(double x, double y) {
    double distance = sqrt(pow(x - position.x, 2) + pow(y - position.y, 2));
    double timetmp = distance / speed;

    std::cout << "Движение к (" << x << ", " << y << "), время: " << timetmp << " сек\n";
    sleep(static_cast<int>(timetmp));

    position = { x, y };
    lastUpdate = time(nullptr);
    if(battery - static_cast<int>(distance)<0)
        emergencyStop();
    else battery - static_cast<int>(distance)<0;
    }

void Robot::changeSpeed(double newSpeed) {
    speed = newSpeed;
    std::cout << "скорость изменена на : " << speed << "\n";
    
}

void Robot::emergencyStop() {
    std::cout << "Аварийная остановка!\n";
    running = false;
}

void Robot::sendStatus() {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "POS:%.1f,%.1f SPD:%.1f BAT:%d",
        position.x, position.y, speed, battery);
    write(fd_control, buffer, strlen(buffer) + 1);
}

void Robot::stop() {
    std::cout << "Сигнал стоп получен.\n Выключение...";
    running = false;
}

int main() {
    try {
        Robot robot;
        robot.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка робота: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}