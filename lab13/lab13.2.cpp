#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#define MAX_FILENAME_LEN 256
#define BUFFER_SIZE 1024

int main() {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char filename[MAX_FILENAME_LEN];
    char buffer[BUFFER_SIZE];
    FILE *file;
    size_t bytes_read;

    printf("Содержимое текущего каталога:\n");
    dir = opendir(".");
    if (dir == NULL) {
        perror("Ошибка открытия каталога");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);

    printf("\nВведите имя файла: ");
    if (fgets(filename, MAX_FILENAME_LEN, stdin) == NULL) {
        perror("Ошибка чтения ввода");
        return 1;
    }

    filename[strcspn(filename, "\n")] = '\0';

    if (stat(filename, &file_stat) == -1) {
        perror("Ошибка получения информации о файле");
        return 1;
    }

    if (file_stat.st_size == 0) {
        printf("Файл '%s' пустой.\n", filename);
        return 0;
    }

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        return 1;
    }

    printf("\nСодержимое файла '%s':\n", filename);
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        fwrite(buffer, 1, bytes_read, stdout);
    }

    fclose(file);
    return 0;
}