// server.cpp
#include "server.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <string.h>

const std::string ControlServer::FIFO_ROBOT = "/tmp/robot_fifo";
const std::string ControlServer::FIFO_CONTROL = "/tmp/control_fifo";

ControlServer::ControlServer() {
    mkfifo(FIFO_ROBOT.c_str(), 0666);
    mkfifo(FIFO_CONTROL.c_str(), 0666);
    fd_robot = open(FIFO_ROBOT.c_str(), O_WRONLY);
    fd_control = open(FIFO_CONTROL.c_str(), O_RDONLY);

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
        char status[256];
    ssize_t n = read(fd_control, status, sizeof(status));
    if (n > 0) {
        status[n] = '\0';
        std::cout << "Статус робота: " << status << "\n";
    }
}

void ControlServer::run(const std::string& trajectoryFile) {
    std::ifstream file(trajectoryFile);
    if (!file) {
        throw std::runtime_error("Failed to open trajectory file");
    }


    Robot::Command initCmd{ Robot::CommandType::CHANGE_SPEED, 0, 0, 1.5, time(nullptr) };
    sendCommand(initCmd);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;


        double x, y,spd;
        
        sscanf(line.c_str(), "%lf %lf %lf", &x, &y,&spd);
        Robot::Command initCmd{ Robot::CommandType::CHANGE_SPEED, 0, 0, spd, time(nullptr) };
        sendCommand(initCmd);

        Robot::Command moveCmd{ Robot::CommandType::MOVE, x, y, 0, time(nullptr) };
        sendCommand(moveCmd);


        sleep(1); 
    }

    Robot::Command stopCmd{ Robot::CommandType::STOP, 0, 0, 0, time(nullptr) };
    sendCommand(stopCmd);
}
int main() {
    try {
        ControlServer server;
        server.run("trajectory.txt");
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}