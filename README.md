# Argparse: Minimal argument parser in C

This library implements a minimal/simple argument parsing for C
programs. It is inspired by Python's [argparse](https://docs.python.org/3/library/argparse.html) library.

## Usage

In your C file include the 

```c
#include "argparse.h
```

and define and the array

```c
Arg args[] = {
		[0].type     = ARG_STRING,
		[0].flag     = "--name",
		[0].required = true,
		[0].nargs    = 1,
		[0].help     = "The name of the person",

		[1].type     = ARG_STRING,
		[1].flag     = "--surnames",
		[1].required = false,
		[1].help     = "The surnames of the person (Could be more that 1)",
		 
		[2].type     = ARG_LONG,
		[2].flag     = "--age",
		[2].required = true,
		[2].nargs    = 1,
		[2].help     = "The age of the person",

		[3].type     = ARG_BOOL,
		[3].flag     = "--left_handed",
		[3].required = true,
		[3].help     = "Specify that the person is left-handed. If not specified then the person is considered right-handed",

		[4].type     = ARG_DOUBLE,
		[4].flag     = "--weight",
		[4].required = true,
		[4].nargs    = 1,
		[4].help     = "The weight of the person",

		[5].type     = ARG_DOUBLE,
		[5].flag     = "--grades",
		[5].required = false,
		[5].help     = "The list of grades of the person (Could be more that 1)",
	};
```


### Command-Line Arguments Types
There are 4 types of commandline arguments:

* **ARG_BOOL**: Specifies a flag (e.g., *--enable_optimizations*)

* **ARG_STRING**: Specifies a list of string  type argmuments (e.g., *--files file1.txt file2.txt*)

* **ARG_LONG**: Specifies a list of long type argmuments (e.g., *--numbers 1 2 3 1000 10001*)

* **ARG_DOUBLE**: Specifies a list of double type argmuments (e.g., *--numbers 1 2 3 1234 12345*)

### Number of arguments
* The **ARG_BOOL** does not expect any command-line input arguments thus
  *.nargs* is ignored.

* The **ARG_STRING/ARG_LONG/ARG_DOUBLE** expect input command-line
  arguments. If none is provided then an error is raised. By explicitly
  setting *.nargs = x* only x arguments are allowed. If  *.nargs = 0*
  then an arbitarty number of arguments can be passed.

### Required arguments

* The **ARG_BOOL** can be present or not, thus *.required* is always
  ignored and set to false.

* The **ARG_STRING/ARG_LONG/ARG_DOUBLE** type of arguments can be made
  mandatory or option by setting *.required = true/false*


### Help message
*.help* field provided a brief description of the expected usage of the
command-line argument, when then *--help* flag is passed to the program.


### Parsing the arguments
After declaring the command-line arguments the parsing is performed as
follows:

```c
size_t args_size = sizeof(args)/sizeof(args[0]);
argParse(argc, argv, args, args_size);
```


### Accessing the parsed arguments

After parsing the command-line arguments the parsed data can be
retrieved as follows:

```c
char**   string_args = args[i].data.array_string.items // FOR ARG_STRING
long*    long_args   = args[i].data.array_long.items // FOR ARG_LONG
double*  double_args = args[i].data.array_double.items // FOR ARG_DOUBLE
bool     bool_arg    = args[i].data.exists // FOR ARG_BOOL
```

## Dependencies
**Argparse** depends on [nob.h](https://github.com/tsoding/nob.h/)
library. The nob.h file is included in the *lib/include* folder.

## Integration to projects
The lib folder provides a simple *CMakeList.txt* file that allows you to
integrate **argparse** to your project using CMake. If you prefer to use
another build tool all the *.c* files are placed in the *lib/source*
folder while the include files are in the *lib/include* folder.

## Limitations
* Positional arguments are not supported
* Flag alias are not supported (e.g. *--verbose* aliased to *-v*)
* Specifing a valid range of nargs is not supported
* Many more features are missing
