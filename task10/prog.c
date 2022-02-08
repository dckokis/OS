#include <stdio.h>

int main(int argc, char *argv[]) {
    int i = 1;
    printf("Program was run and got the string: ");
    while (argv[++i] != NULL)
        printf("%s\n", argv[i]);
    return 0;
}

