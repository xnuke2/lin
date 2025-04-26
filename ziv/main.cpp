#include "time_monitor.h"
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // Дочерний процесс - монитор
        TimeMonitor monitor;
        monitor.startMonitoring();
        return 0;
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
        // Ошибка fork
        return 1;
    }

    return 0;
}
g++ - o monitor logger.cpp msgqueue.cpp task_executor.cpp time_monitor.cpp main.cpp
. / monitor