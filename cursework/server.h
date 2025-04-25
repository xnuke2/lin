// server.h


#include <string>
#include <ctime>
#include "robot.h"

class ControlServer {
public:
    ControlServer();
    ~ControlServer();

    void run(const std::string& trajectoryFile);
    void sendCommand(const Robot::Command& cmd);

private:
    int fd_robot;
    int fd_control;
    static const std::string FIFO_ROBOT;
    static const std::string FIFO_CONTROL;
};

 // SERVER_H