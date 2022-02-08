#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    char *programName = argv[1];
    if (argc < 3) {
        printf("Incorrect number of arguments, minimum 3 required, but %d was passed\n", argc);
        return 1;
    }
    pid_t child_process = fork();
    if (child_process == -1) {
        perror("fork() error:");
        return 1;
    } else if (child_process == 0) {
        execv(programName, argv);
        perror("execv() error");
        return 1;
    } else if (child_process > 0) {
        int status;
        pid_t ChildPid;
        do {
            ChildPid = waitpid(child_process, &status, 0);
            if (ChildPid == -1) {
                perror("waitpid() error");
                return 1;
            }
            if (WIFEXITED(status)) {
                printf("Exit code %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Closed by signal: %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("Stopped by signal: %d\n", WSTOPSIG(status));
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 0;
}