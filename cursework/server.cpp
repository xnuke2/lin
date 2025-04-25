// server.cpp
#include "server.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

const std::string ControlServer::FIFO_ROBOT = "/tmp/robot_fifo";
const std::string ControlServer::FIFO_CONTROL = "/tmp/control_fifo";

ControlServer::ControlServer() {
    fd_robot = open(FIFO_ROBOT.c_str(), O_WRONLY);
    fd_control = open(FIFO_CONTROL.c_str(), O_RDONLY | O_NONBLOCK);

    if (fd_robot == -1 || fd_control == -1) {
        throw std::runtime_error("Failed to open FIFO pipes");
    }
}

ControlServer::~ControlServer() {
    close(fd_robot);
    close(fd_control);
}

void ControlServer::sendCommand(const Robot::Command& cmd) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%d %lf %lf %lf %ld",
        static_cast<int>(cmd.type), cmd.x, cmd.y, cmd.speed, cmd.timestamp);
    write(fd_robot, buffer, strlen(buffer) + 1);
}

void ControlServer::run(const std::string& trajectoryFile) {
    std::ifstream file(trajectoryFile);
    if (!file) {
        throw std::runtime_error("Failed to open trajectory file");
    }

    // Начальная настройка
    Robot::Command initCmd{ Robot::CommandType::CHANGE_SPEED, 0, 0, 1.5, time(nullptr) };
    sendCommand(initCmd);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        double x, y;
        sscanf(line.c_str(), "%lf %lf", &x, &y);

        Robot::Command moveCmd{ Robot::CommandType::MOVE, x, y, 0, time(nullptr) };
        sendCommand(moveCmd);

        // Чтение статуса
        char status[256];
        ssize_t n = read(fd_control, status, sizeof(status));
        if (n > 0) {
            status[n] = '\0';
            std::cout << "Статус робота: " << status << "\n";
        }
        sleep(1); // Задержка между командами
    }

    Robot::Command stopCmd{ Robot::CommandType::EMERGENCY_STOP, 0, 0, 0, time(nullptr) };
    sendCommand(stopCmd);
}