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

size_t fileLen(int file) {
    off_t seekPos = lseek(file, 0, SEEK_END);
    if (seekPos == (off_t) -1) {
        return -1;
    }
    return (int) seekPos + 1;
}

typedef struct {
    size_t *storage;
    size_t amountOfElems;
    size_t capacity;
} smartArray;

smartArray *init(size_t size) {
    smartArray *block = (smartArray *) malloc(sizeof(block));
    if (block == NULL)
        return block;
    block->storage = (size_t *) malloc(size * sizeof(size_t));
    if (block->storage == NULL) {
        free(block);
        return NULL;
    }
    block->amountOfElems = 0;
    block->capacity = size;
    return block;
}

bool add(smartArray *vec, size_t elem) {
    if (vec->amountOfElems == vec->capacity) {
        vec->capacity *= 2;
        size_t *data = (size_t *) realloc(vec->storage, vec->capacity * sizeof(size_t));
        if (data == NULL) {
            vec->capacity /= 2;
            return false;
        }
        vec->storage = data;
    }
    vec->storage[vec->amountOfElems++] = elem;
    return true;
}

void destroy(smartArray *vec) {
    free(vec->storage);
    vec->storage = NULL;
    vec->amountOfElems = vec->capacity = 0;
}

bool offsetConstructor(smartArray *offsetArray, const char *buffer, size_t len) {
    int i = 0;
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
    /*open file*/
    int file = open(argv[1], O_RDONLY);
    if (file == -1) {
        printf("File can't be read\n");
        return 1;
    }
    /*file len*/
    size_t len = fileLen(file);
    if (len < 0) {
        printf("Failed to seek file\n");
        closeFile(file);
        return 1;
    }
    /*read file*/
    lseek(file, 0, SEEK_SET);
    char *buffer = (char *) mmap(0, len, PROT_READ, MAP_SHARED, file, 0);
    if (buffer == NULL) {
        printf("Unable to allocate buffer to save file in\n");
        closeFile(file);
        return 1;
    }
    if (!readFile(file, buffer, len)) {
        closeFile(file);
        if (munmap(buffer, len) == -1) {
            perror("munmap():");
        }
        return 1;
    }
    /*buffer parsing*/
    smartArray *offsetArray = init(10);
    if (offsetArray == NULL) {
        printf("Memory allocation failed\n");
        closeFile(file);
        if (munmap(buffer, len) == -1) {
            perror("munmap():");
        }
        return 1;
    }
    if (!offsetConstructor(offsetArray, buffer, len)) {
        printf("Failed to allocate storage\n");
        closeFile(file);
        destroy(offsetArray);
        if (munmap(buffer, len) == -1) {
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
                    lseek(file, begin, SEEK_SET);
                    char str[255];
                    size_t lineLen = read(file, str, end - begin);
                    if (lineLen < 0 && errno != EINTR) {
                        printf("Failed to read line\n");
                        destroy(offsetArray);
                        closeFile(file);
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
        write(STDIN_FILENO, buffer, len);
    }
    destroy(offsetArray);
    if (munmap(buffer, len) == -1) {
        perror("munmap():");
    }
    closeFile(file);
    return 0;
}