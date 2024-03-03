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

## `getopt`

_**Note:** This section talks about the **Linux version** of `getopt`._

`getopt` takes one of the three following forms:

```bash
getopt optstring parameters
getopt [options] [--] optstring parameters
getopt [options] -o|--options optstring [options] [--] parameters
```

Options:

```
-a, --alternative             allow long options starting with single -
-l, --longoptions <longopts>  the long options to be recognized
-n, --name <progname>         the name under which errors are reported
-o, --options <optstring>     the short options to be recognized
-q, --quiet                   disable error reporting by getopt(3)
-Q, --quiet-output            no normal output
-s, --shell <shell>           set quoting conventions to those of <shell>
-T, --test                    test for getopt(1) version
-u, --unquoted                do not quote the output

-h, --help                    display this help
-V, --version                 display version
```

The `optstring` is a string of recognized option characters. If a character is followed by a colon (`:`), the option requires an argument. The `parameters` is the list of command line arguments to be parsed.

After running `getopt`, the arguments will be permuted: all the option arguments will appear first, followed by `--`, and then the non-option arguments. The output can then be easily parsed by the shell.

### Examples

Here are two examples provided by the `getopt` manual page, written in Bash and Tcsh syntax respectively, located in the `/usr/share/doc/util-linux/examples` directory.

#### [Bash version](./examples/shell/getopt-example.bash)

```bash
#!/bin/bash

# A small example script for using the getopt(1) program.
# This script will only work with bash(1).
# A similar script using the tcsh(1) language can be found
# as getopt-example.tcsh.

# Example input and output (from the bash prompt):
#
# ./getopt-example.bash -a par1 'another arg' --c-long 'wow!*\?' -cmore -b " very long "
# Option a
# Option c, no argument
# Option c, argument 'more'
# Option b, argument ' very long '
# Remaining arguments:
# --> 'par1'
# --> 'another arg'
# --> 'wow!*\?'

# Note that we use "$@" to let each command-line parameter expand to a
# separate word. The quotes around "$@" are essential!
# We need TEMP as the 'eval set --' would nuke the return value of getopt.
TEMP=$(getopt -o 'ab:c::' --long 'a-long,b-long:,c-long::' -n 'example.bash' -- "$@")

if [ $? -ne 0 ]; then
	echo 'Terminating...' >&2
	exit 1
fi

# Note the quotes around "$TEMP": they are essential!
eval set -- "$TEMP"
unset TEMP

while true; do
	case "$1" in
		'-a'|'--a-long')
			echo 'Option a'
			shift
			continue
		;;
		'-b'|'--b-long')
			echo "Option b, argument '$2'"
			shift 2
			continue
		;;
		'-c'|'--c-long')
			# c has an optional argument. As we are in quoted mode,
			# an empty parameter will be generated if its optional
			# argument is not found.
			case "$2" in
				'')
					echo 'Option c, no argument'
				;;
				*)
					echo "Option c, argument '$2'"
				;;
			esac
			shift 2
			continue
		;;
		'--')
			shift
			break
		;;
		*)
			echo 'Internal error!' >&2
			exit 1
		;;
	esac
done

echo 'Remaining arguments:'
for arg; do
	echo "--> '$arg'"
done
```

#### [Tcsh version](./examples/shell/getopt-example.tcsh)

```tcsh
#!/bin/tcsh

# A small example script for using the getopt(1) program.
# This script will only work with tcsh(1).
# A similar script using the bash(1) language can be found
# as getopt-example.bash.

# Example input and output (from the tcsh prompt):
# ./getopt-example.tcsh -a par1 'another arg' --c-long 'wow\!*\?' -cmore -b " very long "
# Option a
# Option c, no argument
# Option c, argument `more'
# Option b, argument ` very long '
# Remaining arguments:
# --> `par1'
# --> `another arg'
# --> `wow!*\?'

# Note that we had to escape the exclamation mark in the wow-argument. This
# is _not_ a problem with getopt, but with the tcsh command parsing. If you
# would give the same line from the bash prompt (ie. call ./parse.tcsh),
# you could remove the exclamation mark.

# This is a bit tricky. We use a temp variable, to be able to check the
# return value of getopt (eval nukes it). argv contains the command arguments
# as a list. The ':q`  copies that list without doing any substitutions:
# each element of argv becomes a separate argument for getopt. The braces
# are needed because the result is also a list.
set temp=(`getopt -s tcsh -o ab:c:: --long a-long,b-long:,c-long:: -- $argv:q`)
if ($? != 0) then
  echo "Terminating..." >/dev/stderr
  exit 1
endif

# Now we do the eval part. As the result is a list, we need braces. But they
# must be quoted, because they must be evaluated when the eval is called.
# The 'q` stops doing any silly substitutions.
eval set argv=\($temp:q\)

while (1)
	switch($1:q)
	case -a:
	case --a-long:
		echo "Option a" ; shift
		breaksw;
	case -b:
	case --b-long:
		echo "Option b, argument "\`$2:q\' ; shift ; shift
		breaksw
	case -c:
	case --c-long:
		# c has an optional argument. As we are in quoted mode,
		# an empty parameter will be generated if its optional
		# argument is not found.

		if ($2:q == "") then
			echo "Option c, no argument"
		else
			echo "Option c, argument "\`$2:q\'
		endif
		shift; shift
		breaksw
	case --:
		shift
		break
	default:
		echo "Internal error!" ; exit 1
	endsw
end

echo "Remaining arguments:"
# foreach el ($argv:q) created problems for some tcsh-versions (at least
# 6.02). So we use another shift-loop here:
while ($#argv > 0)
	echo '--> '\`$1:q\'
	shift
end
```