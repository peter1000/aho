#ifndef TEST_H
#define TEST_H

#include "common.h"


/*
 * Fail a test and print a message.
 */
void fail(char *file, unsigned int line, char *format, ...);


/*
 * Pass a test.
 */
void pass(void);


#endif
