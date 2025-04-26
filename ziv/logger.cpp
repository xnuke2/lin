#include "time_monitor.h"
#include <sstream>
#include <iomanip>
#include <string.h>

Logger::Logger(const std::string& filename) : log_file(filename) {
    fd = open(log_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        throw "Failed to open log file";
    }
}

Logger::~Logger() {
    if (fd != -1) {
        close(fd);
    }
}

void Logger::writeToLog(int stage, double allowed_time, double actual_time) {
    time_t now = time(NULL);
    struct tm* tm = localtime(&now);
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", tm);

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
        "%s Stage %d: Allowed=%.3fs, Actual=%.3fs\n",
        timestamp, stage, allowed_time, actual_time);

    if (write(fd, buffer, strlen(buffer)) == -1) {
        // Ошибка записи
    }
}