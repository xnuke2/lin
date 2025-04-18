#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define THREADS_PER_BATCH 5
#define DELAY_BETWEEN_BATCHES 2

int pipe_fd[2]; 


void* thread_work(void* arg) {
    int thread_id = *(int*)arg;
    int work_time = 1 + rand() % 5; 
    sleep(work_time);

    char msg[100];
    int len = snprintf(msg, sizeof(msg), "Поток %d завершил работу (%d сек)\n", thread_id, work_time);
    write(pipe_fd[1], msg, len); 

    free(arg);
    return NULL;
}


void* pipe_reader(void* arg) {
    char buf[1024];
    while (1) {
        ssize_t bytes_read = read(pipe_fd[0], buf, sizeof(buf));
        if (bytes_read > 0) {
            write(STDOUT_FILENO, buf, bytes_read); 
        }
    }
}

int main() {
    srand(time(NULL));
    int thread_counter = 0;
    time_t last_batch_time = time(NULL);

    if (pipe(pipe_fd) == -1) {
        perror("Ошибка создания pipe");
        return 1;
    }


    pthread_t reader_thread;
    pthread_create(&reader_thread, NULL, pipe_reader, NULL);
    pthread_detach(reader_thread);

    printf("Родительский процесс (PID: %d)\n", getpid());
    int counter =1;
    while (1) {
        
        for (int i = 0; i < THREADS_PER_BATCH; i++) {
            int* tmp =(int*)malloc(sizeof(int));
            *tmp = counter++;
            pthread_t tid;
            if (pthread_create(&tid, NULL, thread_work, tmp) != 0) {
                perror("Ошибка создания потока");
                free(tmp);
            }
            pthread_detach(tid);
        }
        
        sleep(2); 
    }

    return 0;
}