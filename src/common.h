/*
 * common.h: Shared functions and macro definitions
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL.
 *
 * Created by David Čepelík <david@cepelik.eu> in 2014.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#define DEBUG(...) printf(__VA_ARGS__);
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))
#define MAX(a, b)  ((a) < (b) ? (b) : (a))


typedef unsigned int uint;
typedef unsigned int unicode;


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
