# Parsing Arguments in C

Here we describe two ways of parsing command line arguments in C:

1. [Using the `getopt` function](#using-the-getopt-function)
2. Using the `argp` interface

## Using the `getopt` function

The `getopt` family includes the three following functions:

| Function | Header File | Remarks |
|----------|-------------|---------|
| <code>_int_ **getopt** _(int argc, char *const *argv, const char *options)_</code> | `<unistd.h>` | Parse short options (options with a single letter). |
| <code>_int_ **getopt_long** _(int argc, char *const *argv, const char *shortopts, const struct option *longopts, int *indexptr)_</code> | `<getopt.h>` | Parse short options and long options. Short options begin with a single dash (`-`); long options begin with two dashes (`--`). |
| <code>_int_ **getopt_long_only** _(int argc, char *const *argv, const char *shortopts, const struct option *longopts, int *indexptr)_</code> | `<getopt.h>` | Works like `getopt_long`, but it allows long options to begin with a single dash (`-`). If an option that starts with `-` (not `--`) doesn't match a long option, but does match a short option, it is parsed as a short option instead. |

When parsing arguments, the `getopt`, `getopt_long` and `getopt_long_only` functions are called repeatedly so that every command line argument can be processed. When the processing is completed, these functions finally return -1 to indicate that no more option arguments are present.

There are also four global variables that you can use with `#include <unistd.h>`:

```c
extern char *optarg;
extern int optind, opterr, optopt;
```

For brevity, we will not present the details of these functions and variables here. You can go to the [GNU libc manual](https://www.gnu.org/software/libc/manual/html_node/Getopt.html) website to read the complete documentation.

Here we will only present the examples to show you how these functions work:

* [`getopt` example](#getopt-example)
* [`getopt_long` example](#getopt_long-example)

### Examples

#### `getopt` Example

This is a simple program that illustrates how to use `getopt`. It takes two options: `-a` and `-b`. The `-b` option requires an argument. The program also prints the remaining arguments after the options.

> Source: [`examples/c/getopt/main.c`](./examples/c/getopt/main.c)

```c
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
```

#### `getopt_long` Example

The program specifies the following options:

* Long options: `--add`, `--append`, `--delete`, `--verbose`, `--create`, `--file`
* Short options: `-a`, `-b`, `-c`, `-d`, `-0`, `-1`, `-2`

The long option `--create` and the short option `-c` are equivalent.

> Source: [`examples/c/getopt_long/main.c`](./examples/c/getopt_long/main.c)

```c
#include <getopt.h>
#include <stdio.h>  /* for printf */
#include <stdlib.h> /* for exit */

int main(int argc, char **argv) {
    int c;
    int digit_optind = 0;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"add", required_argument, 0, 0},
            {"append", no_argument, 0, 0},
            {"delete", required_argument, 0, 0},
            {"verbose", no_argument, 0, 0},
            {"create", required_argument, 0, 'c'},
            {"file", required_argument, 0, 0},
            {0, 0, 0, 0}};

        c = getopt_long(argc, argv, "abc:d:012", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 0:
            printf("option %s", long_options[option_index].name);
            if (optarg)
                printf(" with arg %s", optarg);
            printf("\n");
            break;

        case '0':
        case '1':
        case '2':
            if (digit_optind != 0 && digit_optind != this_option_optind)
                printf("digits occur in two different argv-elements.\n");
            digit_optind = this_option_optind;
            printf("option %c\n", c);
            break;

        case 'a':
            printf("option a\n");
            break;

        case 'b':
            printf("option b\n");
            break;

        case 'c':
            printf("option c with value '%s'\n", optarg);
            break;

        case 'd':
            printf("option d with value '%s'\n", optarg);
            break;

        case '?':
            break;

        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}
```

## References

* [Program Arguments](https://www.gnu.org/software/libc/manual/html_node/Program-Arguments.html): The GNU libc manual that describes how libc handles program arguments.
    * [Program Argument Syntax Conventions](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html)
    * [Parsing Program Arguments](https://www.gnu.org/software/libc/manual/html_node/Parsing-Program-Arguments.html)
        * [Parsing Program Arguments Using `getopt`](https://www.gnu.org/software/libc/manual/html_node/Getopt.html)
            * [Using the `getopt` function](https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html)
            * [Example of Parsing Arguments with `getopt`](https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html)
            * [Parsing Long Options with `getopt_long`](https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html)
            * [Example of Parsing Long Options with `getopt_long`](https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html)
