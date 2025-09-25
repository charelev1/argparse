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


#ifndef ARGPARSE_H_
#define ARGPARSE_H_

#include <stdio.h>
#include <stdbool.h>

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
	bool           exists;       // used for ARG_BOOL
	ArrayOfStrings array_string; // used for ARG_STRING
	ArrayOfLongs   array_long;   // used for ARG_LONG
	ArrayOfDoubles array_double; // used for ARG_DOUBLE
} ArgData;


typedef enum {
	ARG_BOOL,
	ARG_STRING,
	ARG_LONG,
	ARG_DOUBLE
} ArgType;


typedef struct {
	ArgType     type;
	const char* flag;
	ArgData     data;
	bool        required;
	const char* help;
	size_t      nargs;
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

const char* argTypeToString(ArgType a);
const char* argTokenTypeToString(ArgType a);
bool        tokenizeArguments(int argc, char** argv, ArgTokens* tokens);
bool        parseFlagNargs(const ArgTokens* tokens, size_t* current_count, Args* args_parsed);
bool        parseFlagBool(const ArgTokens* tokens, size_t* current_count, Args* args_parsed);
bool        checkParsedArguments(const Args* args_parsed, Arg* args, size_t args_size);
bool        argParse(int argc, char** argv, Arg* args, size_t args_size);

#endif // ARGPARSE_H_
