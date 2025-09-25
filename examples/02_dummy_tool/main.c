#include <stdio.h>
#include <stdbool.h>
#include "argparse.h"

// Calling from shell examples: 
// ./02_dummy_tool  --input_files hello.c world.c --output main.o --optimization
// ./02_dummy_tool  --input_files foo.c bar.c baz.c --output main.o 

int main(int argc, char** argv)
{
	Arg args[] = {
		[0].type     = ARG_STRING,
		[0].flag     = "--input_files",
		[0].required = true,
		[0].nargs    = 0, // if .type != ARG_BOOL setting nargs = 0 means that nargs = inf
		[0].help     = "Input C files",

		[1].type     = ARG_BOOL,
		[1].flag     = "--optimization",
		[1].nargs    = 14,   // if .type == ARG_BOOL  nargs = 14 is ignored since the flag type is ARG_BOOL
		[1].required = true, // if .type == ARG_BOOL then required=true is ignored and treated as if it was set to false
		[1].help     = "Enable optimizations or not",

		[2].type     = ARG_STRING,
		[2].flag     = "--output",
		[2].required = true,
		[2].nargs    = 1,
		[2].help     = "Output object file ",
		 
	};

	size_t args_size = sizeof(args)/sizeof(args[0]);
	argParse(argc, argv, args, args_size);

	// parsed --input_files 
	Arg a = args[0];

	for (size_t i = 0; i < a.data.array_string.count; i++) {
		char* file_name = a.data.array_string.items[i];
		printf("file_name = %s\n", file_name);
	}

	// parsed --optimization 
	a = args[1];
	if(a.data.exists == true) {
		printf("flag = %s is enabled!\n", a.flag);
	}

	// parsed --output 
	a = args[2];
	printf("output file = %s\n", a.data.array_string.items[0]);


}
