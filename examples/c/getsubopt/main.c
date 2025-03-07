#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int optind;
extern char *optarg;

int do_all;
const char *type;
int read_size;
int write_size;
int read_only;

enum { RO_OPTION = 0, RW_OPTION, READ_SIZE_OPTION, WRITE_SIZE_OPTION, THE_END };

const char *mount_opts[] = {[RO_OPTION] = "ro",
                            [RW_OPTION] = "rw",
                            [READ_SIZE_OPTION] = "rsize",
                            [WRITE_SIZE_OPTION] = "wsize",
                            [THE_END] = NULL};

int main(int argc, char **argv) {
    char *subopts, *value;
    int opt;

    while ((opt = getopt(argc, argv, "at:o:")) != -1)
        switch (opt) {
        case 'a':
            do_all = 1;
            break;
        case 't':
            type = optarg;
            break;
        case 'o':
            subopts = optarg;
            while (*subopts != '\0')
                switch (getsubopt(&subopts, mount_opts, &value)) {
                case RO_OPTION:
                    read_only = 1;
                    break;
                case RW_OPTION:
                    read_only = 0;
                    break;
                case READ_SIZE_OPTION:
                    if (value == NULL)
                        abort();
                    read_size = atoi(value);
                    break;
                case WRITE_SIZE_OPTION:
                    if (value == NULL)
                        abort();
                    write_size = atoi(value);
                    break;
                default:
                    /* Unknown suboption. */
                    printf("Unknown suboption `%s'\n", value);
                        break;
                }
            break;
        default:
            abort();
        }

    /* Do the real work. */

    return 0;
}