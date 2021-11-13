#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "smartArray.h"

typedef struct _FILE {
    int descriptor;
    int len;
    char *content;
    char* error_message;
} File;

void closeFile(int fd) {
    int ret = close(fd);
    if (ret == -1)
        printf("Could not close file\n");
}

bool readFile(int file, char *buffer, size_t len) {
    size_t readSize = read(file, buffer, len);
    if (readSize < 0 && errno != EINTR) {
        printf("Failed to read file\n");
        return false;
    }
    return true;
}
File *fileProcessing(char *filename) {
    File *file = malloc(sizeof(File));
    file->descriptor = open(filename, O_RDONLY);
    off_t seekPos = lseek(file->descriptor, 0, SEEK_END);
    if (seekPos == (off_t) -1) {
        file->len = -1;
    }
    file->len = (int) seekPos + 1;
    lseek(file->descriptor, 0, SEEK_SET);
    file->content = (char *) mmap(0, file->len, PROT_READ, MAP_SHARED, file->descriptor, 0);
    return file;
}

int fileCheck(File *file) {
    if (file->descriptor == -1) {
        file->error_message = "File can't be read\n";
        return 2;
    }
    if (file->len < 0) {
        file->error_message = "Failed to seek file\n";
        return 1;
    }
    if (file->content == NULL) {
        file->error_message = "Unable to allocate buffer to save file in\n";
        return 1;
    }
    return 0;
}

bool offsetConstructor(smartArray *offsetArray, const char *buffer, size_t len) {
    size_t i = 0;
    for (i = 0; i < len; ++i) {
        if (buffer[i] == '\n')
            if (!add(offsetArray, i + 1)) {
                return false;
            }
    }
    return true;
}

int wait() {
    fd_set rfds;
    struct timeval tv;
    int input;
    /*wait for user input*/
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    input = select(1, &rfds, NULL, NULL, &tv);
    return input;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: ./a.out 'filename'\nOr wait for 5 sec to see all text\n");
        return 0;
    }
    File *file = fileProcessing(argv[1]);
    int err_code = fileCheck(file);
    if (err_code > 0) {
        printf("%s", file->error_message);
        if(err_code == 2) {
            return 1;
        }
        closeFile(file->descriptor);
        if (munmap(file->content, file->len) == -1) {
            perror("munmap():");
        }
        free(file);
        return 1;
    }
    /*buffer parsing*/
    smartArray *offsetArray = init(10);
    if (offsetArray == NULL) {
        printf("Memory allocation failed\n");
        closeFile(file->descriptor);
        if (munmap(file->content, file->len) == -1) {
            perror("munmap():");
        }
        free(file);
        return 1;
    }
    if (!offsetConstructor(offsetArray, file->content, file->len)) {
        printf("Failed to allocate storage\n");
        closeFile(file->descriptor);
        destroy(offsetArray);
        if (munmap(file->content, file->len) == -1) {
            perror("munmap():");
        }
        return 1;
    }
    /*runtime*/
    bool exit = false;
    int input = wait();
    /*if input than do smth*/
    if (input) {
        while (!exit) {
            int strNumber;
            if (scanf("%d", &strNumber) != 1) {
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                printf("Incorrect input\n");
                continue;
            }
            if (strNumber == 0)
                exit = true;
            else {
                if (strNumber < 0 || strNumber > offsetArray->amountOfElems)
                    printf("Out of range\n");
                else {
                    size_t begin;
                    size_t end;
                    if (strNumber == 1) {
                        begin = 0;
                    } else {
                        begin = offsetArray->storage[strNumber - 2];
                    }
                    end = offsetArray->storage[strNumber - 1];
                    lseek(file->descriptor, begin, SEEK_SET);
                    char str[255];
                    size_t lineLen = read(file->descriptor, str, end - begin);
                    if (lineLen < 0 && errno != EINTR) {
                        printf("Failed to read line\n");
                        destroy(offsetArray);
                        closeFile(file->descriptor);
                        if (munmap(file->content, file->len) == -1) {
                            perror("munmap():");
                        }
                        free(file);
                        return 0;
                    }
                    str[end - begin - 1] = '\0';
                    printf("%s\n", str);
                }
            }
        }
    }
        /*else print all*/
    else {
        write(STDIN_FILENO, file->content, file->len);
    }
    destroy(offsetArray);
    if (munmap(file->content, file->len) == -1) {
        perror("munmap():");
    }
    closeFile(file->descriptor);
    free(file);
    return 0;
}