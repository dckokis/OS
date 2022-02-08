#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
extern char **environ;


int execvpe(const char *file, char* const argv[], char* envp[]) {
    if (clearenv()) {
        perror("clearenv() error:");
        return -1;
    }
    environ = envp;
    if (execvp(file, argv)) {
        perror("execvp(2) error:");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    char **backup = environ;
    char *args[] = {"Argument 1", "Argument 2", "Argument 3", NULL};
    char *envp[] = {"EnvironmentVar1=Help", "EnvironmentVar2=Me", "EnvironmentVar3=PLEASE", NULL};
    if (execvpe(argv[1], args, envp) == -1) {
        perror("execvpe(3) error:");
        environ = backup;
        return -1;
    }
    return 0;
}