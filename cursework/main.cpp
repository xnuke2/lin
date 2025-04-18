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
#define SPEED 1.0  // ���������� �������� (������/���)
#define BUFFER_SIZE 256

typedef struct {
    double x;
    double y;
} Point;

// ����� �������� (�������������)
const Point LOAD_POINT = { 0.0, 0.0 };

// ������� ���������� ���������� ����� �������
double calculate_distance(Point a, Point b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

// ������� ������
void robot_process() {
    Point current = LOAD_POINT;
    Point target;
    char buffer[BUFFER_SIZE];

    // ������� ����������� ������
    mkfifo(FIFO_ROBOT, 0666);
    mkfifo(FIFO_CONTROL, 0666);

    int fd_robot = open(FIFO_ROBOT, O_RDONLY);
    int fd_control = open(FIFO_CONTROL, O_WRONLY);

    if (fd_robot == -1 || fd_control == -1) {
        perror("������ �������� FIFO");
        exit(1);
    }

    printf("����� ����� ������ � ����� (%.1f, %.1f)\n", current.x, current.y);
    printf("�������� ��������� �� ����������...\n");

    while (1) {
        // ������ ���������� �� ����������
        int n = read(fd_robot, buffer, BUFFER_SIZE);
        if (n <= 0) break;

        // ��������� ������� ����������
        if (strncmp(buffer, "STOP", 4) == 0) {
            printf("�������� ������� ���������\n");
            break;
        }

        sscanf(buffer, "%lf %lf", &target.x, &target.y);

        double distance = calculate_distance(current, target);
        double time = distance / SPEED;

        printf("�������� � ����� (%.1f, %.1f), ����� � ����: %.1f ���\n",
            target.x, target.y, time);

        sleep((int)time);  // �������� ��������
        current = target;

        // ���������� ������������� � ���������� �����
        write(fd_control, "ARRIVED", 8);
    }

    close(fd_robot);
    close(fd_control);
    unlink(FIFO_ROBOT);
    unlink(FIFO_CONTROL);
    printf("����� �������� ������\n");
}

// ������� ���������� (������ ���������� �� �����)
void control_process() {
    FILE* trajectory = fopen(TRAJECTORY_FILE, "r");
    if (!trajectory) {
        perror("������ �������� ����� ����������");
        exit(1);
    }

    int fd_robot = open(FIFO_ROBOT, O_WRONLY);
    int fd_control = open(FIFO_CONTROL, O_RDONLY);

    if (fd_robot == -1 || fd_control == -1) {
        perror("������ �������� FIFO");
        fclose(trajectory);
        exit(1);
    }

    printf("������� ���������� ����� ������\n");
    printf("������ ��������� �� ����� %s...\n", TRAJECTORY_FILE);

    char buffer[BUFFER_SIZE];
    char response[10];
    int point_count = 0;

    while (fgets(buffer, BUFFER_SIZE, trajectory)) {
        // ���������� ������ ������ � �����������
        if (buffer[0] == '\n' || buffer[0] == '#') continue;

        // ������� ������ ����� ������
        buffer[strcspn(buffer, "\n")] = '\0';

        printf("�������� ���������: %s\n", buffer);
        write(fd_robot, buffer, strlen(buffer) + 1);

        // ���� ������������� �� ������
        read(fd_control, response, sizeof(response));
        if (strncmp(response, "ARRIVED", 7) == 0) {
            printf("����� ������ ����� #%d\n", ++point_count);
        }
    }

    // ���������� ������� ����������
    write(fd_robot, "STOP", 5);

    fclose(trajectory);
    close(fd_robot);
    close(fd_control);
    printf("��� ����� ���������� ���������� (%d �����)\n", point_count);
}

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // �������� ������� - �����
        robot_process();
    }
    else if (pid > 0) {
        // ������������ ������� - ����������
        sleep(1);  // ���� ����� ������� FIFO
        control_process();

        // ��������� ������� ������
        kill(pid, SIGTERM);
        wait(NULL);
    }
    else {
        perror("������ ��� �������� ��������");
        return 1;
    }

    return 0;
}