#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 10

// Структура для sembuf
struct sembuf sb;

// Функция для работы с семафорами
void sem_op(int semid, int sem_num, int op) {
    sb.sem_num = sem_num;
    sb.sem_op = op;
    sb.sem_flg = 0;
    semop(semid, &sb, 1);
}

int main() {
    key_t key = ftok("shmfile", 65); 
    int shmid = shmget(key, sizeof(int) * BUFFER_SIZE, 0666 | IPC_CREAT); 
    int *buffer = (int *)shmat(shmid, NULL, 0); 

    int semid = semget(key, 3, 0666 | IPC_CREAT); 
    semctl(semid, 0, SETVAL, BUFFER_SIZE); //empty
    semctl(semid, 1, SETVAL, 0); // full
    semctl(semid, 2, SETVAL, 1); // mutex

    if (fork() == 0) {
        while (1) {
            sleep(2); 

            sem_op(semid, 1, -1); // full--
            sem_op(semid, 2, -1); // mutex--

            int item = buffer[0];
            printf("Потребитель: прочитал %d\n", item);

                for (int i = 0; i < BUFFER_SIZE - 1; i++) {
                buffer[i] = buffer[i + 1];
            }

            sem_op(semid, 2, 1); // mutex++
            sem_op(semid, 0, 1); // empty++
        }
    } else {
        // Код для производителя
        int item = 0;
        while (1) {
            sleep(1); // Имитация работы

            // Ожидание свободного места
            sem_op(semid, 0, -1); // empty--
            sem_op(semid, 2, -1); // mutex--

            // Запись данных в буфер
            buffer[item % BUFFER_SIZE] = item;
            printf("Производитель: записал %d\n", item);
            item++;

            sem_op(semid, 2, 1); // mutex++
            sem_op(semid, 1, 1); // full++
        }
    }


    shmdt(buffer);


    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return 0;
}