/*
 * test.c: Minimalist environment for TDD
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL.
 *
 * Created by David Čepelík <david@cepelik.eu> in 2014.
 */

#ifndef TEST_H
#define TEST_H

#include "common.h"


#define TEST_HEADER() printf("\n%s:\n", __FILE__);

#define ASSERT(x)  if (!(x)) fail(__FILE__, __LINE__, "%s", #x); else pass();
#define ASSERT_MATCH(s1, s2)  if (strcmp(s1, s2)) fail(__FILE__, __LINE__, "%s [%s] matches %s [%s]", #s1, s1, #s2, s2); else pass();
#define ASSERT_EQUAL(x, y)  if (x != y)  fail(__FILE__, __LINE__, "(%s) [%i] == (%s) [%i]", #x, x, #y, y); else pass();
#define ASSERT_NOT_NULL(x)  if (x == NULL) fail(__FILE__, __LINE__, "%s is not null", #x); else pass();
#define ASSERT_TRUE(x) if (!x) fail(__FILE__, __LINE__, "%s is true", #x); else pass();
#define ASSERT_FALSE(x) if (x) fail(__FILE__, __LINE__, "%s is false", #x); else pass();
#define FAIL(...) fail(__FILE__, __LINE__, __VA_ARGS__);


/*
 * Fail a test and print a message.
 */
void fail(char *file, uint line, char *format, ...);


/*
 * Pass a test.
 */
void pass(void);


#endif
