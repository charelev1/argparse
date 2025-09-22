//  Argument parser supported arg types:
//  	Simple boolean flag: --flag 
//  	Flag one argumument data:  --file file1.txt 
//  	Flag with many argument data: --files file1.txt file2.txt
//
#include <stdio.h>
#include <stdbool.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

typedef struct {
    char** items;
    size_t count;
    size_t capacity;
} ArrayOfStrings;

typedef struct {
    long* items;
    size_t count;
    size_t capacity;
} ArrayOfLongs;

typedef struct {
    double* items;
    size_t count;
    size_t capacity;
} ArrayOfDoubles;

typedef union {
	bool           exists;       // used for ARG_FLAG_BOOL
	ArrayOfStrings array_string; // used for ARG_FLAG_NARG_STRING
	ArrayOfLongs   array_long;   // used for ARG_FLAG_NARG_LONG
	ArrayOfDoubles array_double; // used for ARG_FLAG_NARG_DOUBLE
} ArgData;


typedef enum {
	ARG_FLAG_BOOL,
	ARG_FLAG_NARG_STRING,
	ARG_FLAG_NARG_LONG,
	ARG_FLAG_NARG_DOUBLE
} ArgType;


typedef struct {
	ArgType     type;
	const char* flag;
	ArgData     data;
	bool        required;
	size_t      nargs;
} Arg;


typedef struct {
       Arg*      items;
       size_t    count;
       size_t    capacity;
} Args;


const char * argTypeToString(ArgType a)
{
	if (a == ARG_FLAG_BOOL)        return "ARG_FLAG_BOOL";
	if (a == ARG_FLAG_NARG_STRING) return "ARG_FLAG_NARG_STRING";
	if (a == ARG_FLAG_NARG_LONG)   return "ARG_FLAG_NARG_LONG";
	if (a == ARG_FLAG_NARG_DOUBLE) return "ARG_FLAG_NARG_DOUBLE";
	nob_log(ERROR, "Unknown argument type %s", a);
	abort();
}


typedef enum {
	ARG_TOKEN_FLAG,
	ARG_TOKEN_DATA
} ArgTokenType;


typedef struct {
	ArgTokenType type;
	const char*  data;
} ArgToken;


typedef struct {
       ArgToken* items;
       size_t    count;
       size_t    capacity;
} ArgTokens;


const char * argTokenTypeToString(ArgType a)
{
	if (a == ARG_TOKEN_FLAG) return "ARG_TOKEN_FLAG";
	if (a == ARG_TOKEN_DATA) return "ARG_TOKEN_DATA";
	nob_log(ERROR, "Unknown argument type %s", a);
	abort();
}

bool tokenizeArguments(int argc, char** argv, ArgTokens* tokens)
{
	ArgToken t;
	for (size_t i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] == '-') {
			t.type = ARG_TOKEN_FLAG;
			t.data = argv[i];
		} else {
			t.type = ARG_TOKEN_DATA;
			t.data = argv[i];
		}
		da_append(tokens, t);
	}
	return 0;
}


bool parseFlagNargs(const ArgTokens* tokens, size_t* current_count, Args* args_parsed)
{
	size_t i = *current_count;
	if(i >= tokens->count)
	{
		nob_log(ERROR, "Current count %d is bigger that the tokens->count %d: ", i, tokens->count);
		abort();
	}

	if (tokens->items[i].type != ARG_TOKEN_FLAG) {
		nob_log(ERROR, "Token %s type is not ARG_TOKEN_FLAG it is of type: %s", tokens->items[i].data,
			argTokenTypeToString(tokens->items[i].type));
		abort();
	}

	if(i+1 >= tokens->count)
		return 1;


	if (!(tokens->items[i+1].type == ARG_TOKEN_DATA)) 
		return 1;

	// ARG_TOKEN_FLAG +2 ARG_TOKEN_DATA

	Arg arg = {0};	
	arg.type = ARG_FLAG_NARG_STRING;
	arg.flag = tokens->items[i++].data;
	ArrayOfStrings a = {0};
	da_append(&arg.data.array_string, (char*)tokens->items[i++].data);

	while (tokens->items[i].type == ARG_TOKEN_DATA && i < tokens->count) {
		da_append(&arg.data.array_string, (char*)tokens->items[i].data);
		i++;
	}
	
	da_append(args_parsed, arg);
	*current_count = i;
	return 0;
}


bool parseFlagBool(const ArgTokens* tokens, size_t* current_count, Args* args_parsed)
{
	size_t i = *current_count;
	if(i >= tokens->count)
	{
		nob_log(ERROR, "Current count %d is bigger that the tokens->count %d: ", i, tokens->count);
		abort();
	}

	if (tokens->items[i].type != ARG_TOKEN_FLAG) {
		nob_log(ERROR, "Token %s type is not ARG_TOKEN_FLAG it is of type: ", tokens->items[i].data, tokens->items[i].type);
		abort();
	}

	// ARG_TOKEN_FLAG
	Arg arg = {0};	
	arg.type = ARG_FLAG_BOOL;
	arg.flag = tokens->items[i++].data;
	arg.data.exists = true;
	da_append(args_parsed, arg);
	*current_count = i;
	return 0;
}


bool parseArguments(const ArgTokens* tokens, Args* args_parsed)
{
	size_t current_count = 0;
	while (current_count < tokens->count) {
		if (!parseFlagNargs(tokens, &current_count, args_parsed)) continue;
		if (!parseFlagBool(tokens, &current_count, args_parsed))  continue;
	}

}


bool checkParsedArguments(const Args* args_parsed)
{
	// Duplicate flags check
	da_foreach(Arg, a, args_parsed) {
		da_foreach(Arg, b, args_parsed) {
			if (a != b && !strcmp(b->flag,a->flag)) {
				nob_log(ERROR, "Duplicate flags %s provided", a->flag);
				abort();
			}
		}
	}
}


bool argParse(int argc, char** argv, Arg* args, size_t args_size)
{
	ArgTokens tokens = {0};
	tokenizeArguments(argc, argv, &tokens);

	//da_foreach(ArgToken, t, &tokens) {
	//     	size_t index = t - tokens.items;
	//	nob_log(INFO, "%d, %s", t->type, t->data);
	//}

	Args args_parsed = {0};
	parseArguments(&tokens, &args_parsed);
	checkParsedArguments(&args_parsed);

	for (size_t i = 0; i < args_size; i++) {
		// Find parsed arg
		Arg* arg_parsed;
		bool found = false;
		da_foreach(Arg, a, &args_parsed) {
			if (!strcmp(args[i].flag, a->flag)) {
				arg_parsed = a;
				found = true;
				break;
			}
		}

		// Required check
		if (!found && args[i].required) {
			nob_log(ERROR, "Input argument %s is required and it is not provided", args[i].flag);
			abort();
		} else if (!found && !args[i].required) {
			continue;
		}

		// Flag type check
		if (args[i].type != arg_parsed->type && args[i].type  == ARG_FLAG_BOOL) {
			nob_log(ERROR, "Declared argument with flag %s and type %s does not match the parsed one with flag %s and %s",
					args[i].flag,
					argTypeToString(args[i].type),
					arg_parsed->flag,
					argTypeToString(arg_parsed->type));
			abort();
		}

		// Nargs check
		if (args[i].type != ARG_FLAG_BOOL && 
				args[i].nargs > 0 &&
				args[i].nargs != arg_parsed->data.array_string.count) {
			nob_log(ERROR, "Declared argument with flag %s and nargs %d does not match  the parsed one with flag %s and nargs %d",
					args[i].flag,
					args[i].nargs,
					arg_parsed->flag,
					arg_parsed->data.array_string.count
				);
			abort();
		}

		// Assign parsed data to output flag
		//

		if (args[i].type == ARG_FLAG_NARG_STRING) {
			args[i].data.array_string = arg_parsed->data.array_string;
		} else if (args[i].type == ARG_FLAG_NARG_LONG) {
			args[i].data.array_long.count = 0;
			da_foreach(char*, str, &arg_parsed->data.array_string) {
				da_append(&args[i].data.array_long, atol(*str));
	  		}   
		} else if (args[i].type == ARG_FLAG_NARG_DOUBLE) {
			args[i].data.array_double.count = 0;
			da_foreach(char*, str, &arg_parsed->data.array_string) {
				da_append(&args[i].data.array_double, atof(*str));
	  		}
		} else if (args[i].type == ARG_FLAG_BOOL) {
			args[i].data.exists = arg_parsed->data.exists;
		}
	}

	//da_foreach(Arg, a, &args_parsed) {
	//	nob_log(INFO, "flag: %s", a->flag);
	//	if (a->type == ARG_FLAG_NARG) {
	//		da_foreach(char*, str, &a->data.array) {
	//			nob_log(INFO, "Data: %s", *str);
	//		}
	//	} else if (a->type == ARG_FLAG_BOOL) {
	//			nob_log(INFO, "bool flag: %d", a->data.exists);
	//	}
	//}
}



int main(int argc, char** argv)
{
	Arg args[] = {
		[0].type     = ARG_FLAG_BOOL,
		[0].flag     = "--test",
		[0].required = true,

		[1].type     = ARG_FLAG_NARG_LONG,
		[1].flag     = "--output",
		[1].nargs    = 2,
		[1].required = true,
		 
		[2].type     = ARG_FLAG_NARG_DOUBLE,
		[2].flag     = "--inputs",
		[2].required = true,
		[2].nargs    = 0
	};

	argParse(argc, argv, args, sizeof(args)/sizeof(args[0]));

	Arg* a = &args[2];

	if (a->type == ARG_FLAG_NARG_STRING) {
		da_foreach(char*, str, &a->data.array_string) {
			nob_log(INFO, "Data: %d", atoll(*str));
		}
	} else if (a->type == ARG_FLAG_NARG_LONG) {
		da_foreach(long, l, &a->data.array_long) {
			nob_log(INFO, "Data: %ld", *l);
		}
	} else if (a->type == ARG_FLAG_NARG_DOUBLE) {
		da_foreach(double, d, &a->data.array_double) {
			nob_log(INFO, "Data: %f", *d);
		}
	} else if (a->type == ARG_FLAG_BOOL) {
			nob_log(INFO, "bool flag: %d", a->data.exists);
	}

}
