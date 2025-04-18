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
#define SPEED 1.0  // постоянная скорость (единиц/сек)
#define BUFFER_SIZE 256

typedef struct {
    double x;
    double y;
} Point;

// Точка погрузки (фиксированная)
const Point LOAD_POINT = { 0.0, 0.0 };

// Функция вычисления расстояния между точками
double calculate_distance(Point a, Point b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

// Процесс робота
void robot_process() {
    Point current = LOAD_POINT;
    Point target;
    char buffer[BUFFER_SIZE];

    // Создаем именованные каналы
    mkfifo(FIFO_ROBOT, 0666);
    mkfifo(FIFO_CONTROL, 0666);

    int fd_robot = open(FIFO_ROBOT, O_RDONLY);
    int fd_control = open(FIFO_CONTROL, O_WRONLY);

    if (fd_robot == -1 || fd_control == -1) {
        perror("Ошибка открытия FIFO");
        exit(1);
    }

    printf("Робот начал работу в точке (%.1f, %.1f)\n", current.x, current.y);
    printf("Ожидание координат от управления...\n");

    while (1) {
        // Читаем координаты от управления
        int n = read(fd_robot, buffer, BUFFER_SIZE);
        if (n <= 0) break;

        // Проверяем команду завершения
        if (strncmp(buffer, "STOP", 4) == 0) {
            printf("Получена команда остановки\n");
            break;
        }

        sscanf(buffer, "%lf %lf", &target.x, &target.y);

        double distance = calculate_distance(current, target);
        double time = distance / SPEED;

        printf("Двигаюсь к точке (%.1f, %.1f), время в пути: %.1f сек\n",
            target.x, target.y, time);

        sleep((int)time);  // Имитация движения
        current = target;

        // Отправляем подтверждение о достижении точки
        write(fd_control, "ARRIVED", 8);
    }

    close(fd_robot);
    close(fd_control);
    unlink(FIFO_ROBOT);
    unlink(FIFO_CONTROL);
    printf("Робот завершил работу\n");
}

// Процесс управления (читает координаты из файла)
void control_process() {
    FILE* trajectory = fopen(TRAJECTORY_FILE, "r");
    if (!trajectory) {
        perror("Ошибка открытия файла траектории");
        exit(1);
    }

    int fd_robot = open(FIFO_ROBOT, O_WRONLY);
    int fd_control = open(FIFO_CONTROL, O_RDONLY);

    if (fd_robot == -1 || fd_control == -1) {
        perror("Ошибка открытия FIFO");
        fclose(trajectory);
        exit(1);
    }

    printf("Процесс управления начал работу\n");
    printf("Чтение координат из файла %s...\n", TRAJECTORY_FILE);

    char buffer[BUFFER_SIZE];
    char response[10];
    int point_count = 0;

    while (fgets(buffer, BUFFER_SIZE, trajectory)) {
        // Пропускаем пустые строки и комментарии
        if (buffer[0] == '\n' || buffer[0] == '#') continue;

        // Удаляем символ новой строки
        buffer[strcspn(buffer, "\n")] = '\0';

        printf("Отправка координат: %s\n", buffer);
        write(fd_robot, buffer, strlen(buffer) + 1);

        // Ждем подтверждения от робота
        read(fd_control, response, sizeof(response));
        if (strncmp(response, "ARRIVED", 7) == 0) {
            printf("Робот достиг точки #%d\n", ++point_count);
        }
    }

    // Отправляем команду завершения
    write(fd_robot, "STOP", 5);

    fclose(trajectory);
    close(fd_robot);
    close(fd_control);
    printf("Все точки траектории обработаны (%d точек)\n", point_count);
}

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // Дочерний процесс - робот
        robot_process();
    }
    else if (pid > 0) {
        // Родительский процесс - управление
        sleep(1);  // Даем время создать FIFO
        control_process();

        // Завершаем процесс робота
        kill(pid, SIGTERM);
        wait(NULL);
    }
    else {
        perror("Ошибка при создании процесса");
        return 1;
    }

    return 0;
}