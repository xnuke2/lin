#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Обработчик сигнала SIGINT
void handle_sigint(int sig) {
    printf("Получен сигнал SIGINT (%d)\n", sig);
}

// Обработчик сигнала SIGUSR1
void handle_sigusr1(int sig) {
    printf("Получен сигнал SIGUSR1 (%d)\n", sig);
}

int main() {
    // Установка обработчиков сигналов
    struct sigaction sa_int, sa_usr1;

    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_usr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    // Блокировка сигналов SIGINT и SIGUSR1
    sigset_t block_mask;
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGINT);
    sigaddset(&block_mask, SIGUSR1);
    
    if (sigprocmask(SIG_BLOCK, &block_mask, NULL) == -1) {
        perror("Ошибка установки сигнальной маски");
        exit(EXIT_FAILURE);
    }

    printf("Сигналы SIGINT и SIGUSR1 заблокированы. PID процесса: %d\n", getpid());
    printf(" kill -SIGINT %d или kill -SIGUSR1 %d\n", getpid(), getpid());
    printf(" kill -SIGKILL %d\n", getpid());

    // Бесконечный цикл для ожидания сигналов
    while (true) {
        sleep(1);
    }

    return 0;
}