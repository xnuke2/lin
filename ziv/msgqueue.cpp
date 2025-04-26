#include "time_monitor.h"
#include <cstdlib>

MessageQueue::MessageQueue() {
    key_t key = ftok(".", PROJECT_ID);
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        throw "Failed to create message queue";
    }
}

MessageQueue::~MessageQueue() {
    msgctl(msgid, IPC_RMID, NULL);
}

void MessageQueue::sendMessage(const TimeMessage& msg) {
    if (msgsnd(msgid, &msg, sizeof(TimeMessage) - sizeof(long), 0) == -1) {
        throw "Failed to send message";
    }
}

void MessageQueue::receiveMessage(TimeMessage& msg) {
    if (msgrcv(msgid, &msg, sizeof(TimeMessage) - sizeof(long), 1, 0) == -1) {
        throw "Failed to receive message";
    }
}