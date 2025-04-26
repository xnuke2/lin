#include "time_monitor.h"
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>

double TaskExecutor::getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void TaskExecutor::simulateWork(double seconds) {
    usleep(seconds * 1000000);
}

TaskExecutor::TaskExecutor(const std::vector<double>& times) : allowed_times(times) {}

void TaskExecutor::executeTask() {
    srand(time(NULL));

    for (size_t i = 0; i < allowed_times.size(); ++i) {
        double start = getCurrentTime();

        // Имитация работы
        double delay = (rand() % 3000) / 1000.0;
        simulateWork(delay);

        double end = getCurrentTime();
        double actual_time = end - start;

        if (actual_time > allowed_times[i]) {
            MessageQueue::TimeMessage msg;
            msg.msg_type = 1;
            msg.stage = i + 1;
            msg.actual_time = actual_time;
            msg.allowed_time = allowed_times[i];
            mq.sendMessage(msg);
        }

        std::cout << "Stage " << i + 1 << ": " << actual_time
            << "s (allowed " << allowed_times[i] << "s)\n";
    }

    // Сигнал завершения
    MessageQueue::TimeMessage end_msg;
    end_msg.msg_type = 1;
    end_msg.stage = -1;
    mq.sendMessage(end_msg);
}