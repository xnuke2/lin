#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int shared_var = 10;  
sem_t semaphore;       


void* thread_double(void* arg) {
    while (1) {
        sem_wait(&semaphore);  
        printf("[Поток 1] До: %d → ", shared_var);
        shared_var *= 2;
        printf("После: %d\n", shared_var);
        sem_post(&semaphore); 
        sleep(1+rand()%2);  
    }
    return NULL;
}

void* thread_subtract(void* arg) {
    while (1) {
        sem_wait(&semaphore);  
        printf("[Поток 2] До: %d → ", shared_var);
        shared_var -= 2;
        printf("После: %d\n", shared_var);
        sem_post(&semaphore);  
        sleep(1+rand()%2);  
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    sem_init(&semaphore, 0, 1);

    pthread_create(&t1, NULL, thread_double, NULL);
    pthread_create(&t2, NULL, thread_subtract, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&semaphore);  
    return 0;
}