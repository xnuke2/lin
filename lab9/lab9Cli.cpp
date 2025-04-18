#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h> // Для getpid()

#define MSG_SIZE 128

// Структура для сообщения
struct message {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    key_t key;
    int msgid;
    struct message msg;
    pid_t pid = getpid(); 

    
    key = ftok("msg_queue", 65);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    
    msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    
    msg.msg_type = 1;
    strcpy(msg.msg_text, "Hello from client");
    if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    printf("Message sent to server\n");

    
    if (msgrcv(msgid, &msg, sizeof(msg.msg_text), 2, 0) == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    printf("Server says: %s\n", msg.msg_text);

    
    char filename[256];
    snprintf(filename, sizeof(filename), "client_log.%d", pid);

    
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "Received: %s\n", msg.msg_text);
    fclose(file);

    printf("Message written to %s\n", filename);

    return 0;
}