//  Argument parser supported arg types:
//  	Simple boolean flag: --flag 
//  	Flag one argumument data:  --file file1.txt 
//  	Flag with many argument data: --files file1.txt file2.txt
//
#include <stdio.h>
#include <stdbool.h>

#include "argparse.h"
#define NOB_STRIP_PREFIX
#include "nob.h"


int main(int argc, char** argv)
{
	Arg args[] = {
		[0].type     = ARG_BOOL,
		[0].flag     = "--test",
		[0].required = false,
		[0].help     = "This a help message for 0",

		[1].type     = ARG_LONG,
		[1].flag     = "--output",
		[1].nargs    = 2,
		[1].required = false,
		[1].help     = "This a help message for 1",
		 
		[2].type     = ARG_DOUBLE,
		[2].flag     = "--inputs",
		[2].required = false,
		[2].help     = "This a help message for 2",

		[3].type     = ARG_STRING,
		[3].flag     = "--name",
		[3].required = false,
	};

	argParse(argc, argv, args, sizeof(args)/sizeof(args[0]));

	Arg a = args[3];

	if (a.type == ARG_STRING) {
		da_foreach(char*, str, &a.data.array_string) {
			nob_log(INFO, "Data: %s", *str);
		}
	} else if (a.type == ARG_LONG) {
		da_foreach(long, l, &a.data.array_long) {
			nob_log(INFO, "Data: %ld", *l);
		}
	} else if (a.type == ARG_DOUBLE) {
		da_foreach(double, d, &a.data.array_double) {
			nob_log(INFO, "Data: %f", *d);
		}
	} else if (a.type == ARG_BOOL) {
			nob_log(INFO, "bool flag: %d", a.data.exists);
	}
}
