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

int blankCounter() {
    FILE *pipe;
    if ((pipe = (FILE *) popen("grep ^$ file.txt | wc -l", "r")) == NULL) {
        perror("popen():");
        return 1;
    }
    char buffer[MAX_SIZE];
    char *amountOfEmpty;
    char *endPtr;
    if ((amountOfEmpty = fgets(buffer, MAX_SIZE, pipe)) == NULL && !feof(pipe)) {
        perror("fgets():");
        return 1;
    }
    int status = pclose(pipe);
    if (status == -1) {
        perror("pclose():");
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
    printf("Count of blank lines is %ld\n", strtol(amountOfEmpty, &endPtr, 10));
    return 0;
}

int main() {
    return blankCounter();
}