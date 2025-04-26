
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

class Logger {
private:
    int fd;
    std::string log_file;

public:
    Logger(const std::string& filename = "time_control.log") : log_file(filename) {
        fd = open(log_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1) {
            throw "Failed to open log file";
        }
    }

    ~Logger() {
        if (fd != -1) close(fd);
    }

    void writeToLog(int stage, double allowed_time, double actual_time) {
        time_t now = time(NULL);
        struct tm* tm = localtime(&now);
        char timestamp[100];
        strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", tm);

        char buffer[256];
        snprintf(buffer, sizeof(buffer),
            "%s Stage %d: Allowed=%.3fs, Actual=%.3fs\n",
            timestamp, stage, allowed_time, actual_time);

        write(fd, buffer, strlen(buffer));
    }
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

    MessageQueue() {
        key_t key = ftok(".", PROJECT_ID);
        msgid = msgget(key, 0666 | IPC_CREAT);
        if (msgid == -1) {
            throw "Failed to create message queue";
        }
    }

    ~MessageQueue() {
        msgctl(msgid, IPC_RMID, NULL);
    }

    void sendMessage(const TimeMessage& msg) {
        if (msgsnd(msgid, &msg, sizeof(TimeMessage) - sizeof(long), 0) == -1) {
            throw "Failed to send message";
        }
    }

    void receiveMessage(TimeMessage& msg) {
        if (msgrcv(msgid, &msg, sizeof(TimeMessage) - sizeof(long), 1, 0) == -1) {
            throw "Failed to receive message";
        }
    }
};

class TaskExecutor {
private:
    std::vector<double> allowed_times;
    MessageQueue mq;

    double getCurrentTime() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec + tv.tv_usec / 1000000.0;
    }

    void simulateWork(double seconds) {
        usleep(seconds * 1000000);
    }

public:
    TaskExecutor(const std::vector<double>& times) : allowed_times(times) {}

    void executeTask() {
        srand(time(NULL));

        for (size_t i = 0; i < allowed_times.size(); ++i) {
            double start = getCurrentTime();

            // Имитация работы
            double delay = (rand() % 3000) / 1000.0;
            simulateWork(delay);

            double end = getCurrentTime();
            double actual_time = end - start;

            if (actual_time > allowed_times[i]) {
                MessageQueue::TimeMessage msg = { 1, static_cast<int>(i + 1), actual_time, allowed_times[i] };
                mq.sendMessage(msg);
            }

            std::cout << "Stage " << i + 1 << ": " << actual_time
                << "s (allowed " << allowed_times[i] << "s)\n";
        }

        // Сигнал завершения
        MessageQueue::TimeMessage end_msg = { 1, -1 };
        mq.sendMessage(end_msg);
    }
};

class TimeMonitor {
private:
    Logger logger;
    MessageQueue mq;

public:
    TimeMonitor() {}

    void startMonitoring() {
        std::cout << "Time monitor started. Waiting for messages...\n";

        while (true) {
            MessageQueue::TimeMessage msg;
            mq.receiveMessage(msg);

            if (msg.stage == -1) {
                std::cout << "Received end signal. Exiting.\n";
                break;
            }

            logger.writeToLog(msg.stage, msg.allowed_time, msg.actual_time);
            std::cout << "Logged stage " << msg.stage << " ("
                << msg.actual_time << "s > " << msg.allowed_time << "s)\n";
        }
    }
};

// Функция для запуска всей системы
static void runTimeMonitorSystem() {
    pid_t pid = fork();

    if (pid == 0) {
        // Дочерний процесс - монитор
        TimeMonitor monitor;
        monitor.startMonitoring();
        exit(0);
    }
    else if (pid > 0) {
        // Родительский процесс - задача
        sleep(1); // Даем время монитору запуститься

        double times[] = { 1.0, 2.0, 1.5, 3.0 };
        std::vector<double> allowed_times(times, times + sizeof(times) / sizeof(times[0]));

        TaskExecutor task(allowed_times);
        task.executeTask();

        wait(NULL); // Ждем завершения дочернего процесса
    }
    else {
        throw "Fork failed";
    }
}



int main() {
    try {
        runTimeMonitorSystem();
    }
    catch (const char* msg) {
        std::cerr << "Error: " << msg << std::endl;
        return 1;
    }
    return 0;
}