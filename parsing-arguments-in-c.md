# Parsing Arguments in C

Here we describe two ways of parsing command line arguments in C:

1. [Using the `getopt` function](#using-the-getopt-function)
2. [Using the Argp interface](#using-the-argp-interface)

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

## Using the Argp Interface

The GNU C Library also provides the Argp interface to parse command line arguments. The Argp utilities are more feature-rich than the `getopt` family functions. For example, Argp can automatically generate output messages in response to the `--help` and `--usage` options, as described in the GNU coding standards, so that using Argp makes programmers less likely to neglect implementing these options or to keep them up-to-date.

Argp also provides the ability to merge several independently defined option parsers into one, mediating conflicts between them and making the result appear seamless. A library can export an Argp option parser that user programs might employ in conjunction with their own option parsers, resulting in less work for the user programs. Some programs may use only argument parsers exported by libraries, thereby achieving consistent and efficient option-parsing for abstractions implemented by the libraries.

The main interface to Argp is the `argp_parse` function. To use it, you need to `#include <argp.h>`.

For programs with a complex set of options, one single layer of options is usually enough. Hence these programs need to support sub-options so that they can group different sets of related options together to perform various tasks. Hence there is the `getsubopt` function for these programs to handle sub-options.

There is also the `argp_help` function to print help messages. There are also functions that can be used in Argp parsers: `argp_usage`, `argp_error`, `argp_failure` and `argp_state_help`.

| Function | Header File | Remarks |
|----------|-------------|---------|
| <code>_error_t_ **argp_parse** _(const struct argp *argp, int argc, char **argv, unsigned flags, int *arg\_index, void *input)_</code> | `<argp.h>` | Parse arguments in `argv`, of length `argc`, using the parser Argp. The `flags` argument is a bit mask of flags that control the behavior of the parser. The `arg_index` argument is a pointer to an integer that will be set to the index of the first non-option argument in `argv`. The `input` argument is a pointer to a structure that will be filled with the parsed values. |
| <code>_int_ **getsubopt** _(char **optionp, char *const *tokens, char **valuep)_</code> | `<stdlib.h>` | Parse sub-options. It can be used by programs that perform various tasks, where each task has a set of options that are closely related together. |
| <code>_void_ **argp_help** _(const struct argp *argp, FILE *stream, unsigned flags, char *name)_</code> | `<argp.h>` | Print help message. |
| <code>_int_ **argp_usage** _(const struct argp\_state *state)_</code> | `<argp.h>` | Outputs the standard usage message for the Argp parser referred to by state to `state->err_stream` and terminates the program with `exit (argp_err_exit_status)`. |
| <code>_void_ **argp_error** _(const struct argp\_state *state, const char *fmt, …)_</code> | `<argp.h>` | Prints the `printf` format string `fmt` and following args, preceded by the program name and `:`, and followed by a `Try … --help` message, and terminates the program with an exit status of `argp_err_exit_status`. |
| <code>_void_ **argp_failure** _(const struct argp\_state *state, int status, int errnum, const char *fmt, …)_</code> | `<argp.h>` | Similar to the standard GNU error-reporting function error, this prints the program name and `:`, the `printf` format string `fmt`, and the appropriate following args. If it is non-zero, the standard unix error text for `errnum` is printed. If status is non-zero, it terminates the program with that value as its exit status.<br/>The difference between `argp_failure` and `argp_error` is that `argp_error` is for parsing errors, whereas `argp_failure` is for other problems that occur during parsing but don't reflect a syntactic problem with the input, such as illegal values for options, bad phase of the moon, etc. |
| <code>_void_ **argp_state_help** _(const struct argp\_state *state, FILE *stream, unsigned flags)_</code> | `<argp.h>` | Outputs a help message for the Argp parser referred to by `state`, to `stream`. The `flags` argument determines what sort of help message is produced. |

For brevity we will not cover each function in detail here. You can go to the [GNU libc manual](https://www.gnu.org/software/libc/manual/html_node/Argp.html) website to read the complete documentation.

Here we present some examples to show how Argp can be used in different use cases:

* Argp Examples:
    1. [A Minimal Program Using Argp](https://www.gnu.org/software/libc/manual/html_node/Argp-Example-1.html)
    2. [A Program Using Argp with Only Default Options](https://www.gnu.org/software/libc/manual/html_node/Argp-Example-2.html)
    3. [A Program Using Argp with User Options](https://www.gnu.org/software/libc/manual/html_node/Argp-Example-3.html)
    4. [A Program Using Multiple Combined Argp Parsers](https://www.gnu.org/software/libc/manual/html_node/Argp-Example-4.html)
* Parsing of Sub-Options:
    1. [Parsing Sub-Options](https://www.gnu.org/software/libc/manual/html_node/Suboptions.html)

## References

* [Program Arguments](https://www.gnu.org/software/libc/manual/html_node/Program-Arguments.html): The GNU libc manual that describes how libc handles program arguments.
    * [Program Argument Syntax Conventions](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html)
    * [Parsing Program Arguments](https://www.gnu.org/software/libc/manual/html_node/Parsing-Program-Arguments.html)
        * [Parsing Program Arguments Using `getopt`](https://www.gnu.org/software/libc/manual/html_node/Getopt.html)
        * [Parsing Program Arguments with Argp](https://www.gnu.org/software/libc/manual/html_node/Argp.html)