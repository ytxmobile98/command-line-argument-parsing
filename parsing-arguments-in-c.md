# Parsing Arguments in C

Here we describe two ways of parsing command line arguments in C:

1. [Using the `getopt` function](#using-the-getopt-function)
2. Using the `argp` interface

## Using the `getopt` function

The `getopt` family includes the three following functions:

| Function | Signature | Header File |
|----------|-----------|-------------|
| **`getopt`** | <code>_int_ **getopt** _(int argc, char *const *argv, const char *options)_</code> | `<unistd.h>` |
| **`getopt_long`** | <code>_int_ **getopt_long** _(int argc, char *const *argv, const char *shortopts, const struct option *longopts, int *indexptr)_</code> | `<getopt.h>` |
| **`getopt_long_only`** | <code>_int_ **getopt_long_only** _(int argc, char *const *argv, const char *shortopts, const struct option *longopts, int *indexptr)_</code> | `<getopt.h>` |

There are also four global variables that you can use with `#include <unistd.h>`:

```c
extern char *optarg;
extern int optind, opterr, optopt;
```

In the following sections we will cover each of them one by one.

### The Global Variables

| Variable | Description |
|----------|-------------|
| <code>_int_ **opterr**</code> | If the value of this variable is nonzero, then `getopt` prints an error message to the standard error stream if it encounters an unknown option character or an option with a missing required argument. This is the default behavior. If you set this variable to zero, `getopt` does not print any messages, but it still returns the character `?` to indicate an error. |
| <code>_int_ **optopt**</code> | When `getopt` encounters an unknown option character or an option with a missing required argument, it stores that option character in this variable. You can use this for providing your own diagnostic messages. |
| <code>_int_ **optind**</code> | This variable is the index of the next element of the `argv` array to be processed. It is initialized to 1 by the system, and you should not change it. |
| <code>_char *_ **optarg**</code> | This variable is set by `getopt` to point at the value of the option argument, for those options that accept arguments. |

### The `getopt` Function

```c
int getopt(int argc, char *const *argv, const char *options);
```

The `getopt` function gets the next option argument from the argument list specified by the `argv` and `argc` arguments. Normally these values come directly from the arguments received by `main`.

The options argument is a string that specifies the option characters that are valid for this program. An option character in this string can be followed by a colon (`:`) to indicate that it takes a required argument. If an option character is followed by two colons (`::`), its argument is optional; this is a GNU extension.

`getopt` has three ways to deal with options that follow non-options `argv` elements. The special argument `--` forces in all cases the end of option scanning.

* The default is to permute the contents of argv while scanning it so that eventually all the non-options are at the end. This allows options to be given in any order, even with programs that were not written to expect this.
* If the `options` argument string begins with a hyphen (`-`), this is treated specially. It permits arguments that are not options to be returned as if they were associated with option character `\1`.
* POSIX demands the following behavior: the first non-option stops option processing. This mode is selected by either setting the environment variable `POSIXLY_CORRECT` or beginning the `options` argument string with a plus sign (`+`).

**The `getopt` function returns the option character for the next command line option. When no more option arguments are available, it returns `-1`. There may still be more non-option arguments; you must compare the external variable `optind` against the `argc` parameter to check this.**

If the option has an argument, `getopt` returns the argument by storing it in the variable `optarg`. You don't ordinarily need to copy the `optarg` string, since it is a pointer into the original `argv` array, not into a static area that might be overwritten.

If `getopt` finds an option character in `argv` that was not included in options, or a missing option argument, it returns `?` and sets the external variable `optopt` to the actual option character. If the first character of options is a colon (`:`), then `getopt` returns `:` instead of `?` to indicate a missing option argument. In addition, if the external variable `opterr` is nonzero (which is the default), `getopt` prints an error message.

### The `getopt_long` And `getopt_long_only` Functions

These two functions are used to parse arguments with _long options_.

```c
int getopt_long(int argc, char *const *argv, const char *shortopts, const struct option *longopts, int *indexptr);
int getopt_long_only(int argc, char *const *argv, const char *shortopts, const struct option *longopts, int *indexptr);
```

#### The `option` structure

The `option` structure has the following members:

```c
struct option {
    const char *name;
    int         has_arg;
    int        *flag;
    int         val;
};
```

| Field | Description |
|-------|-------------|
| <code>_const char *_ **name**</code> | This field is the name of the option. It is a string. |
| <code>_int_ **has_arg**</code> | This field says whether the option takes an argument. It is an integer, and there are three legitimate values: `no_argument`, `required_argument` and `optional_argument`. |
| <code>_int *_ **flag**</code><br/><code>_int_ **val**</code> | <p>These fields control how to report or act on the option when it occurs.</p><p>If `flag` is a null pointer, then the `val` is a value which identifies this option. Often these values are chosen to uniquely identify particular long options.</p><p>If `flag` is not a null pointer, it should be the address of an `int` variable which is the flag for this option. The value in `val` is the value to store in the flag to indicate that the option was seen.</p> |

#### The `getopt_long` Function

Decode options from the vector `argv` (whose length is `argc`). The argument `shortopts` describes the short options to accept, just as it does in `getopt`. The argument `longopts` describes the long options to accept (see above).

When `getopt_long` encounters a short option, it does the same thing that `getopt` would do: it returns the character code for the option, and stores the option's argument (if it has one) in `optarg`.

When `getopt_long` encounters a long option, it takes actions based on the `flag` and `val` fields of the definition of that option. The option name may be abbreviated as long as the abbreviation is unique.

If `flag` is a null pointer, then `getopt_long` returns the contents of `val` to indicate which option it found. You should arrange distinct values in the `val` field for options with different meanings, so you can decode these values after `getopt_long` returns. If the long option is equivalent to a short option, you can use the short option's character code in `val`.

If `flag` is not a null pointer, that means this option should just set a flag in the program. The flag is a variable of type `int` that you define. Put the address of the flag in the `flag` field. Put in the `val` field the value you would like this option to store in the flag. In this case, `getopt_long` returns `0`.

For any long option, getopt_long tells you the index in the array longopts of the options definition, by storing it into `*indexptr`. You can get the name of the option with `longopts[*indexptr].name`. So you can distinguish among long options either by the values in their `val` fields or by their indices. You can also distinguish in this way among long options that set flags.

When a long option has an argument, `getopt_long` puts the argument value in the variable `optarg` before returning. When the option has no argument, the value in `optarg` is a null pointer. This is how you can tell whether an optional argument was supplied.

When `getopt_long` has no more options to handle, it returns `-1`, and leaves in the variable `optind` the index in `argv` of the next remaining argument.

## References

* [Program Arguments](https://www.gnu.org/software/libc/manual/html_node/Program-Arguments.html): The GNU libc manual that describes how libc handles program arguments.
    * [Program Argument Syntax Conventions](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html)
    * [Parsing Program Arguments](https://www.gnu.org/software/libc/manual/html_node/Parsing-Program-Arguments.html)
        * [Parsing Program Arguments Using `getopt`](https://www.gnu.org/software/libc/manual/html_node/Getopt.html)
            * [Using the `getopt` function](https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html)
            * [Example of Parsing Arguments with `getopt`](https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html)
            * [Parsing Long Options with `getopt_long`](https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html)
            * [Example of Parsing Long Options with `getopt_long`](https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html)
