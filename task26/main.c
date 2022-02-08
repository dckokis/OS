#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 256

int process(FILE *input) {
    char buffer[MAX_SIZE];
    size_t count = fread(buffer, sizeof(char), MAX_SIZE, input);
    fwrite(buffer, count, 1, stdout);
    if (ferror(input) == -1) {
        perror("fread(3)");
        int status = pclose(input);
        if (status == -1) {
            perror("pclose(2):");
            return 1;
        } else {
            if (WIFEXITED(status)) {
                printf("Child terminated normally with exit code %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Killed by signal with number %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("Stopped by signal with number %d\n", WSTOPSIG(status));
            }
        }
        return 1;
    }
    int status = pclose(input);
    if (status == -1) {
        perror("pclose(2):");
        return 1;
    } else {
        if (WIFEXITED(status)) {
            printf("Child terminated normally with exit code %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Killed by signal with number %d\n", WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("Stopped by signal with number %d\n", WSTOPSIG(status));
        }
    }
    size_t i;
    for (i = 0; i < count; ++i) {
        buffer[i] = toupper(buffer[i]);
    }
    fwrite(buffer, count, 1, stdout);

    if (ferror(input) == -1) {
        perror("fwrite(3)");
        return 1;
    }
    return 0;
}

int main() {
    FILE *input = popen("echo hello world", "r");
    if (input == NULL) {
        perror("popen(2)");
        return 1;
    }
    return process(input);
}