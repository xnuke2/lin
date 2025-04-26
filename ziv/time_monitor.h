#include "time_monitor.h"
#include <iostream>

TimeMonitor::TimeMonitor() {}

void TimeMonitor::startMonitoring() {
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