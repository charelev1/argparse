//  Argument parser supported arg types:
//  	Simple boolean flag: --flag 
//  	Flag one argumument data:  --file file1.txt 
//  	Flag with many argument data: --files file1.txt file2.txt
//
#include <stdio.h>
#include <stdbool.h>
#include "argparse.h"

// Calling from shell examples: 
// ./01_person --name George --surnames Black Smith --age 40 --left_handed --weight 90.5 --grades 99.9 89.8
// ./01_person --name George  --age 40  --weight 90.5  

int main(int argc, char** argv)
{
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

	size_t args_size = sizeof(args)/sizeof(args[0]);
	argParse(argc, argv, args, args_size);

	// parsed --name 
	Arg a = args[0];
	const char* flag_name = a.flag;
	const char* name = a.data.array_string.items[0];
	printf("flag_name =%s\n", flag_name);
	printf("name =%s\n", name);

	// parsed --grades 
	a = args[5];

	double* grades = a.data.array_double.items;
	for (size_t i = 0; i < a.data.array_double.count; i++) {
		printf("grade = %lf\n", grades[i]);
	}

	// Print all parsed data
	for (size_t i=0; i < args_size; i++) {
		a = args[i];

		printf("Flag : %s\n", a.flag);

		if (a.type == ARG_STRING) {
			for (size_t j=0; j < a.data.array_string.count; j++) {
				char* str = a.data.array_string.items[j];
				printf("	Data string: %s\n", str);
			}
		} else if (a.type == ARG_LONG) {
			for (size_t j=0; j < a.data.array_double.count; j++) {
				long l = a.data.array_long.items[j];
				printf("	Data long: %ld\n", l);
			}
		} else if (a.type == ARG_DOUBLE) {
			for (size_t j=0; j < a.data.array_double.count; j++) {
				double f = a.data.array_double.items[j];
				printf("	Data double: %f\n", f);
			}
		} else if (a.type == ARG_BOOL) {
				printf("	Data bool: %d\n", a.data.exists);
		}
		printf("\n");
	}
}
