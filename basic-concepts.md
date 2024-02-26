# Basic Concepts

## Command Line Arguments

When you execute a built-in command or a program in a command line shell, you type the **command name**, followed by zero or more **command line arguments**.

For example:

```bash
# Lists all files in the current directory,
# excluding hidden files.
# This execution has no arguments.
ls

# Lists all files in the /usr directory,
# including entries starting with `.`
# The arguments are `-a` and `/usr`.
ls -a /usr
# Or you can use long option `--all`:
ls --all /usr
```

## Options

**Options** are command line arguments that modify the behavior of a command or a program. In Unix-like systems, arguments are treated as options if they begin with a hyphen delimiter `-`. Option names are single alphanumeric characters (as for `isalnum` in C).

Two exceptions:

* `-` is not treated as an option argument. It typically means standard input or standard output.
* `--` is a special argument that terminates all options. From then on all arguments are treated as non-options even if they start with a hyphen.

### Short Options and Long Options

There are two types of options: **short options** and **long options**.

* **Short options** are options with a single character, e.g. `-a`, `-b`, `-c`. Multiple short options can be combined into a single argument, e.g. `-abc` is equivalent to `-a -b -c`.
* **Long options** are options with more than one character, e.g. `--all` and `--author` as in the `ls` program.

### Option Arguments

Certain options may require an argument. For short options and long options, the syntax are as follows:

* **Short options**
    * The option character and arguments may or may not be separated by whitespace. For example, `-o foo` and `-ofoo` are equivalent.
* **Long options**
    * The option and the argument are separated by whitespace or `=`, e.g. `--file foo` or `--file=foo`.
    * _If the argument itself is optional_, the `=` sign must be present to specify the argument, i.e. `--file=foo`.

### Positioning of Options and Non-Option Arguments

* Options typically precede other non-option arguments.
    * The GNU C `getopt` and `argp_parse` functions permute the arguments so that it appear as if all the option arguments were specified before all the non-option arguments for the purposes of parsing, even if the user of your program intermixed option and non-option arguments. _This behavior is nonstandard._
    * The [POSIX standard](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html) requires that non-option arguments strictly follow the options arguments. If you want to suppress the reordering of arguments, defined the `_POSIX_OPTION_ORDER` environment variable. (See: [Standard environment variables](https://www.gnu.org/software/libc/manual/html_node/Standard-Environment.html))
* The special argument `--` terminates all options. From then on all arguments are treated as non-options even if they start with hyphen.
    * For example, `ls -- -l` lists the file named `-l` in the current directory.
* Options may be supplied in any order, or appear multiple times. The interpretation is left up to the particular application program.