#include <stdio.h>
extern char **environ;

int main(int argc, char *argv[]) {
    printf("ARGV PARAMETERS:\n");
    int n;
    char **p;
    for (n = 0; n < argc; ++n) {
        printf("\t%d:'%s'\n", n, argv[n]);
    }
    printf("\nENVIRONMENT VARIABLES:\n");
    for (p = environ; *p; ++p) {
        printf("\t%s\n", *p);
    }
    return 0;
}