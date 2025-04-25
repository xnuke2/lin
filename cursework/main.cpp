#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <signal.h>

#define FIFO_ROBOT "/tmp/robot_fifo"
#define FIFO_CONTROL "/tmp/control_fifo"
#define TRAJECTORY_FILE "trajectory.txt"

#define BUFFER_SIZE 256






void control_process() {
    FILE* trajectory = fopen(TRAJECTORY_FILE, "r");
    if (!trajectory) {
        perror("ошибка открытия файла траектории");
        exit(1);
    }
    mkfifo(FIFO_ROBOT, 0666);
    mkfifo(FIFO_CONTROL, 0666);
    printf("Процесс ожидает подключение\n");
    int fd_robot = open(FIFO_ROBOT, O_WRONLY);
    int fd_control = open(FIFO_CONTROL, O_RDONLY);

    if (fd_robot == -1 || fd_control == -1) {
        perror("Ошибка открытия FIFO");
        fclose(trajectory);
        exit(1);
    }

    printf("Процесс управления начал работу\n");
    printf("Чтение координат из  %s...\n", TRAJECTORY_FILE);

    char buffer[BUFFER_SIZE];
    char response[10];
    int point_count = 0;

    while (fgets(buffer, BUFFER_SIZE, trajectory)) {
        if (buffer[0] == '\n' || buffer[0] == '#') continue;

        buffer[strcspn(buffer, "\n")] = '\0';

        printf("Отправка координат: %s\n", buffer);
        write(fd_robot, buffer, strlen(buffer) + 1);


        read(fd_control, response, sizeof(response));
        if (strncmp(response, "ARRIVED", 7) == 0) {
            printf("Робот достиг точки #%d\n", ++point_count);
        }
    }


    write(fd_robot, "STOP", 5);

    fclose(trajectory);
    close(fd_robot);
    close(fd_control);
    printf("Все точки обработаны (%d точек)\n", point_count);
}

int main() {

    control_process();


    return 0;
}