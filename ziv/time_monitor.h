#ifndef TIME_MONITOR_H
#define TIME_MONITOR_H

#include <string>
#include <vector>
#include <ctime>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>

class Logger {
private:
    int fd;
    std::string log_file;

public:
    Logger(const std::string& filename = "time_control.log");
    ~Logger();

    void writeToLog(int stage, double allowed_time, double actual_time);
};

class MessageQueue {
private:
    int msgid;
    static const int PROJECT_ID = 65;

public:
    struct TimeMessage {
        long msg_type;
        int stage;
        double actual_time;
        double allowed_time;
    };

    MessageQueue();
    ~MessageQueue();

    void sendMessage(const TimeMessage& msg);
    void receiveMessage(TimeMessage& msg);
};

class TaskExecutor {
private:
    std::vector<double> allowed_times;
    MessageQueue mq;

    double getCurrentTime();
    void simulateWork(double seconds);

public:
    TaskExecutor(const std::vector<double>& times);
    void executeTask();
};

class TimeMonitor {
private:
    Logger logger;
    MessageQueue mq;

public:
    TimeMonitor();
    void startMonitoring();
};

#endif