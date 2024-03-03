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

> From: <https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html>

This is a simple program that illustrates how to use `getopt`. It takes two options: `-a` and `-b`. The `-b` option requires an argument. The program also prints the remaining arguments after the options.

> Source: [`examples/c/getopt/main.c`](./examples/c/getopt/main.c)

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int optind;
extern char *optarg;

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

> From: <https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html>

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

For programs with a complex set of options, one single layer of options is usually enough. Hence these programs need to support suboptions so that they can group different sets of related options together to perform various tasks. Hence there is the `getsubopt` function for these programs to handle suboptions.

There is also the `argp_help` function to print help messages. There are also functions that can be used in Argp parsers: `argp_usage`, `argp_error`, `argp_failure` and `argp_state_help`.

| Function | Header File | Remarks |
|----------|-------------|---------|
| <code>_error_t_ **argp_parse** _(const struct argp *argp, int argc, char **argv, unsigned flags, int *arg\_index, void *input)_</code> | `<argp.h>` | Parse arguments in `argv`, of length `argc`, using the parser Argp. The `flags` argument is a bit mask of flags that control the behavior of the parser. The `arg_index` argument is a pointer to an integer that will be set to the index of the first non-option argument in `argv`. The `input` argument is a pointer to a structure that will be filled with the parsed values. |
| <code>_void_ **argp_help** _(const struct argp *argp, FILE *stream, unsigned flags, char *name)_</code> | `<argp.h>` | Print help message. |
| <code>_int_ **argp_usage** _(const struct argp\_state *state)_</code> | `<argp.h>` | Outputs the standard usage message for the Argp parser referred to by state to `state->err_stream` and terminates the program with `exit (argp_err_exit_status)`. |
| <code>_void_ **argp_error** _(const struct argp\_state *state, const char *fmt, …)_</code> | `<argp.h>` | Prints the `printf` format string `fmt` and following args, preceded by the program name and `:`, and followed by a `Try … --help` message, and terminates the program with an exit status of `argp_err_exit_status`. |
| <code>_void_ **argp_failure** _(const struct argp\_state *state, int status, int errnum, const char *fmt, …)_</code> | `<argp.h>` | Similar to the standard GNU error-reporting function error, this prints the program name and `:`, the `printf` format string `fmt`, and the appropriate following args. If it is non-zero, the standard unix error text for `errnum` is printed. If status is non-zero, it terminates the program with that value as its exit status.<br/>The difference between `argp_failure` and `argp_error` is that `argp_error` is for parsing errors, whereas `argp_failure` is for other problems that occur during parsing but don't reflect a syntactic problem with the input, such as illegal values for options, bad phase of the moon, etc. |
| <code>_void_ **argp_state_help** _(const struct argp\_state *state, FILE *stream, unsigned flags)_</code> | `<argp.h>` | Outputs a help message for the Argp parser referred to by `state`, to `stream`. The `flags` argument determines what sort of help message is produced. |

For brevity we will not cover each function in detail here. You can go to the [GNU libc manual](https://www.gnu.org/software/libc/manual/html_node/Argp.html) website to read the complete documentation.

Here we present some examples to show how Argp can be used in different use cases:

* Argp Examples:
    1. [A Minimal Program Using Argp](#example-1-a-minimal-program-using-argp)
    2. [A Program Using Argp with Only Default Options](#example-2-a-program-using-argp-with-only-default-options)
    3. [A Program Using Argp with User Options](#example-3-a-program-using-argp-with-user-options)
    4. [A Program Using Multiple Combined Argp Parsers](#example-4-a-program-using-multiple-combined-argp-parsers)
* Parsing of Suboptions:
    1. [Parsing of Suboptions](#parsing-of-suboptions)

### Examples

#### Example 1: A Minimal Program Using Argp

> From: <https://www.gnu.org/software/libc/manual/html_node/Argp-Example-1.html>

This is a minimal example of a program that uses Argp. It won't do much except give an error message and exit when there are any arguments, and print a (rather pointless) message for `--help`.

> Source: [`examples/c/argp/1_minimal-program-using-argp/main.c`](./examples/c/argp/1_minimal-program-using-argp/main.c)

```c
/* This is (probably) the smallest possible program that
    uses argp.  It won't do much except give an error
    messages and exit when there are any arguments, and print
    a (rather pointless) messages for –help. */

#include <argp.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    argp_parse(0, argc, argv, 0, 0, 0);
    exit(0);
}
```

#### Example 2: A Program Using Argp with Only Default Options

> From: <https://www.gnu.org/software/libc/manual/html_node/Argp-Example-2.html>

This program uses Argp to be compliant with the GNU standard command line format. It doesn't use any options or arguments, but it implements a `--help` option, a `--version` option, and puts the given documentation string and bug address in the `--help` output, as per GNU standards.

> Source: [`examples/c/argp/2_argp-with-default-options/main.c`](./examples/c/argp/2_argp-with-default-options/main.c)

```c
/* This program doesn't use any options or arguments, but uses
    argp to be compliant with the GNU standard command line
    format.

    In addition to making sure no arguments are given, and
    implementing a --help option, this example will have a
    --version option, and will put the given documentation string
    and bug address in the --help output, as per GNU standards.

    The variable ARGP contains the argument parser specification;
    adding fields to this structure is the way most parameters are
    passed to argp_parse (the first three fields are usually used,
    but not in this small program).  There are also two global
    variables that argp knows about defined here,
    ARGP_PROGRAM_VERSION and ARGP_PROGRAM_BUG_ADDRESS (they are
    global variables because they will almost always be constant
    for a given program, even if it uses different argument
    parsers for various tasks). */

#include <argp.h>
#include <stdlib.h>

const char *argp_program_version = "argp-ex2 1.0";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";

/* Program documentation. */
static char doc[] = "Argp example #2 -- a pretty minimal program using argp";

/* Our argument parser.  The options, parser, and
    args_doc fields are zero because we have neither options or
    arguments; doc and argp_program_bug_address will be
    used in the output for '--help', and the '--version'
    option will print out argp_program_version. */
static struct argp argp = {0, 0, 0, doc};

int main(int argc, char **argv) {
    argp_parse(&argp, argc, argv, 0, 0, 0);
    exit(0);
}
```

#### Example 3: A Program using Argp with User Options

> From: <https://www.gnu.org/software/libc/manual/html_node/Argp-Example-3.html>

This program uses the same features as [Example 2](#example-2-a-program-using-argp-with-only-default-options), adding user options and arguments. It uses the first four fields in `argp` and specifies `parse_opt` as the parser function. The `main` function uses a structure to communicate with the `parse_opt` function, a pointer to which it passes in the `input` argument to `argp_parse`.

> Source: [`examples/c/argp/3_argp-with-user-options/main.c`](./examples/c/argp/3_argp-with-user-options/main.c)

```c
/* This program uses the same features as example 2, and uses options and
    arguments.

    We now use the first four fields in ARGP, so here's a description of them:
        OPTIONS  – A pointer to a vector of struct argp_option (see below)
        PARSER   – A function to parse a single option, called by argp
        ARGS_DOC – A string describing how the non-option arguments should look
        DOC      – A descriptive string about this program; if it contains a
                    vertical tab character (\v), the part after it will be
                    printed *following* the options

    The function PARSER takes the following arguments:
        KEY  – An integer specifying which option this is (taken
                from the KEY field in each struct argp_option), or
                a special key specifying something else; the only
                special keys we use here are ARGP_KEY_ARG, meaning
                a non-option argument, and ARGP_KEY_END, meaning
                that all arguments have been parsed
        ARG  – For an option KEY, the string value of its
                argument, or NULL if it has none
        STATE– A pointer to a struct argp_state, containing
                various useful information about the parsing state; used here
                are the INPUT field, which reflects the INPUT argument to
                argp_parse, and the ARG_NUM field, which is the number of the
                current non-option argument being parsed
    It should return either 0, meaning success, ARGP_ERR_UNKNOWN, meaning the
    given KEY wasn't recognized, or an errno value indicating some other
    error.

    Note that in this example, main uses a structure to communicate with the
    parse_opt function, a pointer to which it passes in the INPUT argument to
    argp_parse.  Of course, it's also possible to use global variables
    instead, but this is somewhat more flexible.

    The OPTIONS field contains a pointer to a vector of struct argp_option's;
    that structure has the following fields (if you assign your option
    structures using array initialization like this example, unspecified
    fields will be defaulted to 0, and need not be specified):
        NAME   – The name of this option's long option (may be zero)
        KEY    – The KEY to pass to the PARSER function when parsing this option,
                *and* the name of this option's short option, if it is a
                printable ascii character
        ARG    – The name of this option's argument, if any
        FLAGS  – Flags describing this option; some of them are:
                    OPTION_ARG_OPTIONAL – The argument to this option is optional
                    OPTION_ALIAS        – This option is an alias for the
                                        previous option
                    OPTION_HIDDEN       – Don't show this option in --help output
        DOC    – A documentation string for this option, shown in --help output

    An options vector should be terminated by an option with all fields zero. */

#include <argp.h>
#include <stdlib.h>

const char *argp_program_version = "argp-ex3 1.0";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";

/* Program documentation. */
static char doc[] = "Argp example #3 -- a program with options and arguments using argp";

/* A description of the arguments we accept. */
static char args_doc[] = "ARG1 ARG2";

/* The options we understand. */
static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output"},
    {"quiet", 'q', 0, 0, "Don't produce any output"},
    {"silent", 's', 0, OPTION_ALIAS},
    {"output", 'o', "FILE", 0, "Output to FILE instead of standard output"},
    {0}};

/* Used by main to communicate with parse_opt. */
struct arguments {
    char *args[2]; /* arg1 & arg2 */
    int silent, verbose;
    char *output_file;
};

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
      know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key) {
    case 'q':
    case 's':
        arguments->silent = 1;
        break;
    case 'v':
        arguments->verbose = 1;
        break;
    case 'o':
        arguments->output_file = arg;
        break;

    case ARGP_KEY_ARG:
        if (state->arg_num >= 2)
            /* Too many arguments. */
            argp_usage(state);

        arguments->args[state->arg_num] = arg;

        break;

    case ARGP_KEY_END:
        if (state->arg_num < 2)
            /* Not enough arguments. */
            argp_usage(state);
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {
    struct arguments arguments;

    /* Default values. */
    arguments.silent = 0;
    arguments.verbose = 0;
    arguments.output_file = "-";

    /* Parse our arguments; every option seen by parse_opt will
      be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    printf("ARG1 = %s\nARG2 = %s\nOUTPUT_FILE = %s\n"
        "VERBOSE = %s\nSILENT = %s\n",
        arguments.args[0], arguments.args[1], arguments.output_file,
        arguments.verbose ? "yes" : "no", arguments.silent ? "yes" : "no");

    exit(0);
}
```

#### Example 4: A Program Using Multiple Combined Argp Parsers

> From: <https://www.gnu.org/software/libc/manual/html_node/Argp-Example-4.html>

This program uses the same features as [Example 3](#example-3-a-program-using-argp-with-user-options), but has more options, and presents more structure in the `--help` output. It also illustrates ow you can 'steal' the remainder of the input arguments past a certain point for programs that accept a list of items. It also illustrates the _key_ value `ARGP_KEY_NO_ARGS` which is only given if no non-option arguments were supplied to the program.

> Source: [`examples/c/argp/4_argp-with-user-options/main.c`](./examples/c/argp/4_argp-with-user-options/main.c)

```c
/* This program uses the same features as example 3, but has more
  options, and somewhat more structure in the --help output.  It
  also shows how you can 'steal' the remainder of the input
  arguments past a certain point, for programs that accept a
  list of items.  It also shows the special argp KEY value
  ARGP_KEY_NO_ARGS, which is only given if no non-option
  arguments were supplied to the program.

  For structuring the help output, two features are used,
  *headers* which are entries in the options vector with the
  first four fields being zero, and a two part documentation
  string (in the variable DOC), which allows documentation both
  before and after the options; the two parts of DOC are
  separated by a vertical-tab character ('\v', or '\013').  By
  convention, the documentation before the options is just a
  short string saying what the program does, and that afterwards
  is longer, describing the behavior in more detail.  All
  documentation strings are automatically filled for output,
  although newlines may be included to force a line break at a
  particular point.  All documentation strings are also passed to
  the 'gettext' function, for possible translation into the
  current locale. */

#include <argp.h>
#include <error.h>
#include <stdlib.h>

const char *argp_program_version = "argp-ex4 1.0";
const char *argp_program_bug_address = "<bug-gnu-utils@prep.ai.mit.edu>";

/* Program documentation. */
static char doc[] =
    "Argp example #4 -- a program with somewhat more complicated\
options\
\vThis part of the documentation comes *after* the options;\
 note that the text is automatically filled, but it's possible\
 to force a line-break, e.g.\n<-- here.";

/* A description of the arguments we accept. */
static char args_doc[] = "ARG1 [STRING...]";

/* Keys for options without short-options. */
#define OPT_ABORT 1 /* –abort */

/* The options we understand. */
static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output"},
    {"quiet", 'q', 0, 0, "Don't produce any output"},
    {"silent", 's', 0, OPTION_ALIAS},
    {"output", 'o', "FILE", 0, "Output to FILE instead of standard output"},
    {0, 0, 0, 0, "The following options should be grouped together:"},
    {"repeat", 'r', "COUNT", OPTION_ARG_OPTIONAL, "Repeat the output COUNT (default 10) times"},
    {"abort", OPT_ABORT, 0, 0, "Abort before showing any output"},
    {0},
};

/* Used by main to communicate with parse_opt. */
struct arguments {
    char *arg1;                 /* arg1 */
    char **strings;             /* [string…] */
    int silent, verbose, abort; /* '-s', '-v', '--abort' */
    char *output_file;          /* file arg to '--output' */
    int repeat_count;           /* count arg to '--repeat' */
};

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
        know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key) {
    case 'q':
    case 's':
        arguments->silent = 1;
        break;
    case 'v':
        arguments->verbose = 1;
        break;
    case 'o':
        arguments->output_file = arg;
        break;
    case 'r':
        arguments->repeat_count = arg ? atoi(arg) : 10;
        break;
    case OPT_ABORT:
        arguments->abort = 1;
        break;

    case ARGP_KEY_NO_ARGS:
        argp_usage(state);

    case ARGP_KEY_ARG:
        /* Here we know that state->arg_num == 0, since we
          force argument parsing to end before any more arguments can
          get here. */
        arguments->arg1 = arg;

        /* Now we consume all the rest of the arguments.
            state->next is the index in state->argv of the
            next argument to be parsed, which is the first string
            we're interested in, so we can just use
            &state->argv[state->next] as the value for
            arguments->strings.

            In addition, by setting state->next to the end
            of the arguments, we can force argp to stop parsing here and
            return. */
        arguments->strings = &state->argv[state->next];
        state->next = state->argc;

        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {
    int i, j;
    struct arguments arguments;

    /* Default values. */
    arguments.silent = 0;
    arguments.verbose = 0;
    arguments.output_file = "-";
    arguments.repeat_count = 1;
    arguments.abort = 0;

    /* Parse our arguments; every option seen by parse_opt will be
      reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (arguments.abort)
        error(10, 0, "ABORTED");

    for (i = 0; i < arguments.repeat_count; i++) {
        printf("ARG1 = %s\n", arguments.arg1);
        printf("STRINGS = ");
        for (j = 0; arguments.strings[j]; j++)
            printf(j == 0 ? "%s" : ", %s", arguments.strings[j]);
        printf("\n");
        printf("OUTPUT_FILE = %s\nVERBOSE = %s\nSILENT = %s\n",
            arguments.output_file, arguments.verbose ? "yes" : "no",
            arguments.silent ? "yes" : "no");
    }

    exit(0);
}
```

### Book: _Step-by-Step into Argp_

The book [_Step-by-Step into Argp_](./step-by-step-into-argp/) provides step-by-step guides that cover the details of the Argp details. The code examples are also hosted in the [submodule](https://github.com/ytxmobile98/step-by-step-into-argp/).

## Parsing of Suboptions

Having a single level of options is sometimes not enough. There might be too many options which have to be available or a set of options is closely related. Some programs use **suboptions** to handle these cases.

For each short option (starting with `-`) or long option (starting with `--`), the argument can be a string that contains a list of **suboptions**: suboptions are separated by comma (`,`); some suboptions may have arguments given after the equal sign (`=`). For example: **`ro,name=xyz`**.

The `getsubopt` function is used to parse suboptions. For complete documentation, read the [GNU libc manual](https://www.gnu.org/software/libc/manual/html_node/Suboptions.html).

| Function | Header File | Remarks |
|----------|-------------|---------|
| <code>_int_ **getsubopt** _(char **optionp, char *const *tokens, char **valuep)_</code> | `<stdlib.h>` | Parse suboptions. It can be used by programs that perform various tasks, where each task has a set of options that are closely related together. |

### Example

> From: <https://www.gnu.org/software/libc/manual/html_node/Suboptions-Example.html>

Here is an example of using `getsubopt`, which might appear in the `mount`(8) program.

> Source: [`examples/c/getsubopt/main.c`](./examples/c/getsubopt/main.c)

```c
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
```

## References

* [Program Arguments](https://www.gnu.org/software/libc/manual/html_node/Program-Arguments.html): The GNU libc manual that describes how libc handles program arguments.
    * [Program Argument Syntax Conventions](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html)
    * [Parsing Program Arguments](https://www.gnu.org/software/libc/manual/html_node/Parsing-Program-Arguments.html)
        * [Parsing Program Arguments Using `getopt`](https://www.gnu.org/software/libc/manual/html_node/Getopt.html)
        * [Parsing Program Arguments with Argp](https://www.gnu.org/software/libc/manual/html_node/Argp.html)
        * [Parsing of Suboptions](https://www.gnu.org/software/libc/manual/html_node/Suboptions.html)
* [Book: _Step-by-Step into Argp_](http://nongnu.askapache.com/argpbook/step-by-step-into-argp.pdf)