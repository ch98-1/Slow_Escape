//Copyright (C) 2015  Corwin Hansen
#ifndef CORWINS_H//include protection
	#define CORWINS_H
#include <stdlib.h>

void *ptrval_check_null;

void (*fatal_error)(void);

void SetError(void(*func)(void)){//set function to run if big error occures
	fatal_error = func;
}

#define malloc(x) ptrval_check_null = malloc(x); \
	if (ptrval_check_null == NULL){ \
		printf("Could not allocate memory");\
		fatal_error();\
	}

#define realloc(x, y) ptrval_check_null = realloc(x, y); \
	if (ptrval_check_null == NULL){ \
		printf("Could not allocate memory");\
		fatal_error();\
	}

#define calloc(x, y) ptrval_check_null = calloc(x, y); \
	if (ptrval_check_null == NULL){ \
		printf("Could not allocate memory");\
		fatal_error();\
	}

#endif