#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#define BUFFER_SIZE 256
#define SPEED 1.0  
#define FIFO_ROBOT "/tmp/robot_fifo"
#define FIFO_CONTROL "/tmp/control_fifo"

typedef struct {
    double x;
    double y;
} Point;




const Point LOAD_POINT = { 0.0, 0.0 };


double calculate_distance(Point a, Point b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

void robot_process() {
    Point current = LOAD_POINT;
    Point target;
    char buffer[BUFFER_SIZE];

    


    int fd_robot = open(FIFO_ROBOT, O_RDONLY);
    int fd_control = open(FIFO_CONTROL, O_WRONLY);

    if (fd_robot == -1 || fd_control == -1) {
        perror("error open FIFO");
        exit(1);
    }

    printf("Робот начал в  (%.1f, %.1f)\n", current.x, current.y);
    printf("Ожидание координат...\n");

    while (1) {
        int n = read(fd_robot, buffer, BUFFER_SIZE);
        if (n <= 0) break;

        
        if (strncmp(buffer, "STOP", 4) == 0) {
            printf("Получена команда остановки\n");
            break;
        }

        sscanf(buffer, "%lf %lf", &target.x, &target.y);

        double distance = calculate_distance(current, target);
        double time = distance / SPEED;

        printf("Двигаяс к точке (%.1f, %.1f), время в пути: %.1f сек\n",
            target.x, target.y, time);

        sleep((int)time);  
        current = target;

        
        write(fd_control, "ARRIVED", 8);
    }

    close(fd_robot);
    close(fd_control);
    unlink(FIFO_ROBOT);
    unlink(FIFO_CONTROL);
    printf("Робот завершил работу\n");
}
int main() {

    robot_process();
    return 0;
}