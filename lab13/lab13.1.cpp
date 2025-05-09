#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_LENGTH 1000
#define FILE_PERMISSIONS 0644

void insert_word(char* text, const char* new_word, int position) {
    char buffer[MAX_LENGTH];
    int word_count = 0;
    int i = 0, j = 0;
    int in_word = 0;
    
    while (text[i] != '\0' && word_count < position) {
        if (isspace(text[i])) {
            if (in_word) {
                word_count++;
                in_word = 0;
            }
            buffer[j++] = text[i];
        } else {
            in_word = 1;
            buffer[j++] = text[i];
        }
        i++;
    }
    
    buffer[j] = '\0';
    strcat(buffer, " ");
    strcat(buffer, new_word);
    
    if (text[i] != '\0') {
        strcat(buffer, " ");
        strcat(buffer, &text[i]);
    }
    
    strcpy(text, buffer);
}

int main() {
    char input_text[MAX_LENGTH] = {0};
    char new_word[100] = {0};
    int input_fd, output_fd;
    ssize_t bytes_read;
    
    printf("Введите текст: ");
    fgets(input_text, MAX_LENGTH, stdin);
    input_text[strcspn(input_text, "\n")] = '\0';
    
    printf("Введите слово для добавления: ");
    scanf("%99s", new_word);
    
    input_fd = open("input.txt", O_CREAT | O_WRONLY | O_TRUNC, FILE_PERMISSIONS);
    if (input_fd == -1) {
        perror("Ошибка открытия input.txt");
        return 1;
    }
    write(input_fd, input_text, strlen(input_text));
    close(input_fd);
    
    input_fd = open("input.txt", O_RDONLY);
    if (input_fd == -1) {
        perror("Ошибка открытия input.txt");
        return 1;
    }
    bytes_read = read(input_fd, input_text, MAX_LENGTH - 1);
    if (bytes_read == -1) {
        perror("Ошибка чтения");
        close(input_fd);
        return 1;
    }
    input_text[bytes_read] = '\0';
    close(input_fd);
    
    insert_word(input_text, new_word, 3);
    
    output_fd = open("output.txt", O_CREAT | O_WRONLY | O_TRUNC, FILE_PERMISSIONS);
    if (output_fd == -1) {
        perror("Ошибка открытия output.txt");
        return 1;
    }
    write(output_fd, input_text, strlen(input_text));
    close(output_fd);
    
    printf("Результат успешно записан в output.txt\n");
    
    return 0;
}