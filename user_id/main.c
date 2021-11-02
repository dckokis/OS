#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

void printID() {
    printf("ID : %d\nEID : %d\n",
           getuid(), geteuid());
}

int tryOpen(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("File has not been opened");
    } else {
        fclose(file);
        printf("File opened\n");
        return 0;
    }
}

void run() {
    printID();
    if (tryOpen("test_file") == 0) {
        setuid(getuid());
        printID();
        tryOpen("test_file");
    }
}

int main() {
    run();
    return 0;
}