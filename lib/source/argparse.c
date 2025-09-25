// Copyright 2025 Charalampos Eleftheriadis <charelevresearch@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"
#include "argparse.h"

const char* argTypeToString(ArgType a)
{
	if (a == ARG_BOOL)   return "ARG_BOOL";
	if (a == ARG_STRING) return "ARG_STRING";
	if (a == ARG_LONG)   return "ARG_LONG";
	if (a == ARG_DOUBLE) return "ARG_DOUBLE";
	nob_log(ERROR, "Unknown argument type %s", a);
	exit(1);
}


const char* argTokenTypeToString(ArgType a)
{
	if (a == ARG_TOKEN_FLAG) return "ARG_TOKEN_FLAG";
	if (a == ARG_TOKEN_DATA) return "ARG_TOKEN_DATA";
	nob_log(ERROR, "Unknown argument type %s", a);
	exit(1);
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
		exit(1);
	}

	if (tokens->items[i].type != ARG_TOKEN_FLAG) {
		nob_log(ERROR, "Token %s type is not ARG_TOKEN_FLAG it is of type: %s", tokens->items[i].data,
			argTokenTypeToString(tokens->items[i].type));
		exit(1);
	}

	if(i+1 >= tokens->count)
		return 1;


	if (!(tokens->items[i+1].type == ARG_TOKEN_DATA)) 
		return 1;

	// ARG_TOKEN_FLAG +2 ARG_TOKEN_DATA

	Arg arg = {0};	
	arg.type = ARG_STRING;
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
		exit(1);
	}

	if (tokens->items[i].type != ARG_TOKEN_FLAG) {
		nob_log(ERROR, "Token %s type is not ARG_TOKEN_FLAG it is of type: ", tokens->items[i].data, tokens->items[i].type);
		exit(1);
	}

	// ARG_TOKEN_FLAG
	Arg arg = {0};	
	arg.type = ARG_BOOL;
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


bool checkParsedArguments(const Args* args_parsed, Arg* args, size_t args_size)
{
	da_foreach(Arg, a, args_parsed) {
		// Duplicate flags check
		da_foreach(Arg, b, args_parsed) {
			if (a != b && !strcmp(b->flag,a->flag)) {
				nob_log(ERROR, "Duplicate flags %s provided", a->flag);
				exit(1);
			}
		}

		// Extra undeclared flags check
		bool found = false;
		for (size_t i = 0; i < args_size; i++) {
			if (!strcmp(a->flag,args[i].flag)) {
				found = true;
				break;
			}
		}
		if (!found) {
			nob_log(ERROR, "Undeclared argument flag %s provided of type %s please declared it as an input argument", 
					a->flag,
					argTypeToString(a->type)
					);
			exit(1);
		}
	}

	for (size_t i = 0; i < args_size; i++) {
		for (size_t j = 0; j < args_size; j++) {
			if (i == j) continue;
			if (!strcmp(args[i].flag, args[j].flag)) {
				nob_log(ERROR, "Duplicate flags %s declared. Please declare each flags only 1 time.", args[i].flag); 
				exit(1);
			}
		}
	}
}

void displayHelp(Arg* args, size_t args_size)
{
	printf("Help: Declared arguments:\n");

	for (size_t i = 0; i < args_size; i++) {
		printf("    %s:\n", args[i].flag);
		printf("        type       : %s\n", argTypeToString(args[i].type));
		printf("        required   : %s\n", (args[i].required)?("yes"): "no");
		if (args[i].type != ARG_BOOL) {
			if (args[i].nargs > 0) {
				printf("        nargs      : inf\n");
			} else {
				printf("        nargs      : %d\n",  args[i].nargs);
			}
		}
		if (args[i].help == NULL) {
			printf("\n");
		} else {
			printf("        description: %s\n", args[i].help);
		}


	}
}

bool argParse(int argc, char** argv, Arg* args, size_t args_size)
{
	ArgTokens tokens = {0};
	tokenizeArguments(argc, argv, &tokens);

	Args args_parsed = {0};
	parseArguments(&tokens, &args_parsed);


	if (args_parsed.count == 1) {
		if (!strcmp(args_parsed.items[0].flag, "--help")) {
			displayHelp(args, args_size);
			return 0;
		}
	}

	checkParsedArguments(&args_parsed, args, args_size);

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
			exit(1);
		} else if (!found && !args[i].required) {
			continue;
		}

		// Flag type check
		if ((args[i].type == ARG_BOOL && arg_parsed->type != ARG_BOOL) ||
		    (args[i].type != ARG_BOOL && arg_parsed->type == ARG_BOOL)) {
			nob_log(ERROR, "Declared argument with flag %s and type %s does not match the parsed one with flag %s and %s",
					args[i].flag,
					argTypeToString(args[i].type),
					arg_parsed->flag,
					argTypeToString(arg_parsed->type));
			exit(1);
		}

		// Nargs check
		if (args[i].type != ARG_BOOL && 
				args[i].nargs > 0 &&
				args[i].nargs != arg_parsed->data.array_string.count) {
			nob_log(ERROR, "Declared argument with flag %s and nargs %d does not match the parsed one with flag %s and nargs %d",
					args[i].flag,
					args[i].nargs,
					arg_parsed->flag,
					arg_parsed->data.array_string.count
				);
			exit(1);
		}

		// Assign based on type
		if (args[i].type == ARG_STRING) {
			args[i].data.array_string = arg_parsed->data.array_string;
		} else if (args[i].type == ARG_LONG) {
			args[i].data.array_long.count = 0;
			da_foreach(char*, str, &arg_parsed->data.array_string) {
				const char* tmp = *str;
				for(size_t j = 0; j <strlen(tmp); j++) {
					if ((!isdigit(tmp[j]) && !(j == 0 && tmp[j] == '-'))) {
						nob_log(ERROR, "Declared argument with flag %s and type ARG_LONG does not match the parsed one with flag %s and argument %s",
								args[i].flag,
								arg_parsed->flag,
				tmp
							);
						exit(1);
					}
				}

				da_append(&args[i].data.array_long, atol(tmp));
	  		}   
		} else if (args[i].type == ARG_DOUBLE) {
			args[i].data.array_double.count = 0;
			da_foreach(char*, str, &arg_parsed->data.array_string) {
				const char* tmp = *str;
				bool dotfound = false;
				for(size_t j = 0; j <strlen(tmp); j++) {
					if ((!isdigit(tmp[j]) && !((j == 0 && tmp[j] == '-') || (!dotfound && tmp[j] == '.')))) {
						printf("%c\n", tmp[j]);
						nob_log(ERROR, "Declared argument with flag %s and type ARG_DOUBLE does not match the parsed one with flag %s and argument %s",
								args[i].flag,
								arg_parsed->flag,
								tmp
							);
						exit(1);
					}
					if (tmp[j] == '.') dotfound = true;
				}
				da_append(&args[i].data.array_double, atof(tmp));
	  		}
		} else if (args[i].type == ARG_BOOL) {
			args[i].data.exists = arg_parsed->data.exists;
		}
	}
}
