# Parsing Arguments in Shell

In Linux / Unix systems, command line argument parsing can be done in two ways: using the builtin command `getopts` or the `getopt` program.

* `getopts` is a _builtin command_ originated from the Bourne Shell (`sh`). It is used to process command line arguments following the [POSIX `getopts` utility guidelines](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/getopts.html).
* `getopt` is a program that reads and permutes command line arguments, and outputs permuted arguments that can later be easily parsed and processed by the shell. It is a _standalone program_ that is not part of the shell. There is a _Unix / BSD version_ with limited functionalities, and a Linux version that is more powerful and flexible. For example, the Linux version adds support for mixed options and operands, long options, and optional arguments.

We will cover each of them in the following sections.