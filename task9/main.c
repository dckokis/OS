#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void execute(char *forChild, char *text_for_parent) {
    pid_t pid = fork();
    switch (pid) {
        case (-1):
            perror("fork() error\n");
            break;
        case (0):
            exit(0);
//            execlp("cat", "cat", forChild, NULL);
//            perror("Failed to execute command in child process\n");
            break;
        default:
            sleep(30);
            //while (wait(0) != -1);
            //printf("%s", text_for_parent);
            break;
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./%s 'child_filename'\n", argv[0]);
        return 1;
    }

    char *ch_filename = argv[1];
    char *pr_text = malloc(1000 * sizeof(char));
    if (!pr_text) {
        perror("Failed to allocate memory for parent text\n");
        return 1;
    }
    pr_text = "Hey I am parent\n"
              "Hey I am parent\n"
              "Hey I am parent\n"
              "Hey I am parent\n"
              "Hey I am parent\n"
              "Hey I am parent\n"
              "Hey I am parent\n"
              "Hey I am parent\n"
              "Hey I am parent\n"
              "Hey I am parent\n";
    char *command = malloc(50 * sizeof(char));
    if (!command) {
        perror("Failed to allocate memory for command\n");
        return 1;
    }

    execute(ch_filename, pr_text);
    return 0;
}
