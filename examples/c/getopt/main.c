#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "ab:")) != -1) {
        switch (opt) {
            case 'a':
                printf("Option -a\n");
                break;
            case 'b':
                printf("Option -b with value '%s'\n", optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-a] [-b value]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    for (int i = optind; i < argc; i++) {
        printf("Argument: %s\n", argv[i]);
    }
    return 0;
}