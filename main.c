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

typedef enum {
	ARG_BOOL_FLAG,
	ARG_ONE_FLAG,
	ARG_MANY_FLAGS
} ArgType;


typedef struct {
    char** items;
    size_t count;
    size_t capacity;
} ArrayOfStrings;

typedef union {
	bool           exists;    // used for ARG_BOOL_FLAG
	char*    one_data;  // used for ARG_ONE_FLAG (pointer to single string)
	ArrayOfStrings many_data; // used for ARG_MANY_FLAGS (pointer to pointer of single strings)
} ArgData;


typedef struct {
	ArgType    type;
	const char* flag;
	ArgData    data;
} Arg;

typedef struct {
       Arg*      items;
       size_t    count;
       size_t    capacity;
} Args;



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


bool parseManyFlags(const ArgTokens* tokens, size_t* current_count, Args* args_parsed)
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

	if(i+2 >= tokens->count)
		return 1;


	if (!(tokens->items[i+1].type == ARG_TOKEN_DATA && tokens->items[i+2].type == ARG_TOKEN_DATA)) 
		return 1;

	// ARG_TOKEN_FLAG +2 ARG_TOKEN_DATA

	Arg arg = {0};	
	arg.type = ARG_MANY_FLAGS;
	arg.flag = tokens->items[i++].data;
	ArrayOfStrings a = {0};
	da_append(&arg.data.many_data, (char*)tokens->items[i++].data);
	da_append(&arg.data.many_data, (char*)tokens->items[i++].data);

	while (tokens->items[i].type == ARG_TOKEN_DATA && i < tokens->count) {
		da_append(&arg.data.many_data, (char*)tokens->items[i].data);
		i++;
	}
	
	da_append(args_parsed, arg);
	*current_count = i;
	return 0;
}

bool parseOneFlag(const ArgTokens* tokens, size_t* current_count, Args* args_parsed)
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

	if(i+1 >= tokens->count)
		return 1;

	if (!(tokens->items[i+1].type == ARG_TOKEN_DATA)) 
		return 1;

	// ARG_TOKEN_FLAG + ARG_TOKEN_DATA

	Arg arg = {0};	
	arg.type = ARG_ONE_FLAG;
	arg.flag = tokens->items[i++].data;
	ArrayOfStrings a  = {0};
	arg.data.one_data = (char*)tokens->items[i++].data;
	da_append(args_parsed, arg);
	*current_count = i;
	return 0;
}

bool parseBoolFlag(const ArgTokens* tokens, size_t* current_count, Args* args_parsed)
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
	arg.type = ARG_BOOL_FLAG;
	arg.flag = tokens->items[i++].data;
	arg.data.exists = true;
	da_append(args_parsed, arg);
	*current_count = i;
	return 0;
}


bool parseArguments(const ArgTokens* tokens, Args* args_parsed)
{
	size_t current_count = 0;
	nob_log(INFO, "tokens count %d", tokens->count);
	while (current_count < tokens->count) {
		nob_log(INFO, "current count out %d", current_count);
		if (!parseManyFlags(tokens, &current_count, args_parsed)) continue;
		if (!parseOneFlag(tokens, &current_count, args_parsed))   continue;
		if (!parseBoolFlag(tokens, &current_count, args_parsed))  continue;
	 	//size_t index = t - tokens.items;
		//nob_log(INFO, "%d, %s", t->type, t->data);
	}

}


int main(int argc, char** argv)
{
	Arg args[] = {
		[0].type = ARG_BOOL_FLAG,
		[0].flag = "--test",

		[1].type = ARG_ONE_FLAG,
		[1].flag = "--output",
		 
		[2].type = ARG_MANY_FLAGS,
		[2].flag = "--inputs"
	};

	ArgTokens tokens = {0};
	tokenizeArguments(argc, argv, &tokens);
	da_foreach(ArgToken, t, &tokens) {
	       	size_t index = t - tokens.items;
		nob_log(INFO, "%d, %s", t->type, t->data);
	}

	Args args_parsed = {0};
	parseArguments(&tokens, &args_parsed);

	da_foreach(Arg, a, &args_parsed) {
		nob_log(INFO, "flag: %s", a->flag);
		if (a->type == ARG_MANY_FLAGS) {
			da_foreach(char*, str, &a->data.many_data) {
				nob_log(INFO, "many data: %s", *str);
			}
		} else if (a->type == ARG_ONE_FLAG) {
				nob_log(INFO, "one data: %s", a->data.one_data);
		} else if (a->type == ARG_BOOL_FLAG) {
				nob_log(INFO, "bool flag: %d", a->data.exists);
		}
	}
}
