# Parsing Arguments in Shell

In Linux / Unix systems, command line argument parsing can be done in two ways: using the builtin command `getopts` or the `getopt` program.

* [`getopts`](#getopts) is a _builtin command_ originated from the Bourne Shell (`sh`). It is used to process command line arguments following the [POSIX `getopts` utility guidelines](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/getopts.html).
* `getopt` is a program that reads and permutes command line arguments, and outputs permuted arguments that can later be easily parsed and processed by the shell. It is a _standalone program_ that is not part of the shell. There is a _Unix / BSD version_ with limited functionalities, and a Linux version that is more powerful and flexible. For example, the Linux version adds support for mixed options and operands, long options, and optional arguments.

We will cover each of them in the following sections.

## `getopts`

The `getopts` builtin command has the following syntax:

```sh
getopts optstring name [arg...]
```

* The `optstring` consists of a number of alphanumeric characters as option names. Options requiring an argument is suffixed by a colon (`:`).
    * If the first character in `optstring` is a colon (`:`), the shell variable `OPTARG` shall be set to the option character found, but no output shall be written to standard error; otherwise, the shell variable `OPTARG` shall be unset and a diagnostic message shall be written to standard error.
* The `name` is the name of the variable that will be used to store the option name during each time of processing.

### Example

The Bourne Shell below takes two options: `-a` and `-b`, where `-b` requires an argument. It calls `getopts` repeatedly until all the options are processed. The `opt` variable stores the option name after each processing.

> Source: [`examples/shell/getopts-example.sh`](./examples/shell/getopts-example.sh)

```sh
#!/usr/bin/env sh

while getopts "ab:" opt; do
    case $opt in
        a) echo "Option $opt" ;;
        b) echo "Option $opt: $OPTARG" ;;
        \?) echo "Invalid option: -$opt" ;;
    esac
done
```

Sample use cases:

```bash
$ ./getopts-example.sh -a -b 1
Option a
Option b: 1

$ ./getopts-example.sh -a -b 2 -c
Option a
Option b: 2
Illegal option -c
Invalid option: -?
```