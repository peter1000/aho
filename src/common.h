#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef DEBUG
#undef DEBUG
#define DEBUG(...) printf(__VA_ARGS__);
#else
#define DEBUG(...) 
#endif


#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))


typedef unsigned int uint;


/*
 *  Print a message and quit.
 */
void *die(char *format, ...);


/*
 *  Call malloc() to allocate memory, die() if impossible.
 */
void *malloc_safe(size_t size);


/*
 *  Call realloc() to allocate memory, die() if impossible.
 */
void *realloc_safe(void *x, size_t size);


#endif
