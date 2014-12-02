/*
 * strbuf.test.c: Tests of the growing string buffer
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL.
 *
 * Created by David Čepelík <david@cepelik.eu> in 2014.
 */

#include <string.h>

#include "strbuf.h"
#include "test.h"


strbuf buf;


void
strbuf_test_init(void) {
	strbuf_init(&buf);

	ASSERT(buf.size == STRBUF_INIT_SIZE);
	ASSERT(strlen(buf.str) == 0);

	strbuf_free(&buf);
}


void
strbuf_test_printf(void) {
	strbuf_init(&buf);

	char *tst = "Ahoj, mily Frantisku!";
	strbuf_printf(&buf, 0, "%s", tst);
	ASSERT_MATCH(buf.str, tst);

	strbuf_printf(&buf, 6, "medvede!");
	ASSERT_MATCH(buf.str, "Ahoj, medvede!");

	strbuf_free(&buf);
}


void
strbuf_test_append(void) {
	strbuf_init(&buf);

	strbuf_printf(&buf, 0, "Ahoj, medvede!");
	ASSERT_MATCH(buf.str, "Ahoj, medvede!");

	strbuf_append(&buf, " Jak se mas?");
	ASSERT_MATCH(buf.str, "Ahoj, medvede! Jak se mas?");

	strbuf_free(&buf);
}


void
strbuf_test_invisible(void) {
	strbuf_init(&buf);
	strbuf_append(&buf, "Ahoj, medvede! Jak se mas?");
	
	strbuf_printf(&buf, buf.offset + 1, "invisible");
	ASSERT_MATCH(buf.str, "Ahoj, medvede! Jak se mas?");

	strbuf_append(&buf, "still invisible");
	ASSERT_MATCH(buf.str, "Ahoj, medvede! Jak se mas?");

	strbuf_free(&buf);
}


void
strbuf_test_resize(void) {
	strbuf_init(&buf);
	strbuf_append(&buf, "Ahoj, medvede! Jak se mas?");

	strbuf_resize(&buf, 4);
	ASSERT(buf.size == 4);
	ASSERT(buf.offset == 4);
	ASSERT_MATCH(buf.str, "Ahoj");

	strbuf_free(&buf);
}


void
strbuf_test_growing(void) {
	strbuf_init(&buf);
	strbuf_resize(&buf, 4);

	size_t old_size = buf.size;

	strbuf_printf(&buf, 0, "Petka!");
	ASSERT(buf.size > old_size);
	ASSERT_EQUAL(buf.size, (STRBUF_GROWTH_RATE > 1 ? STRBUF_GROWTH_RATE * old_size : 6));

	size_t next_size = STRBUF_GROWTH_RATE * buf.size;
	char *bad_str = malloc(next_size + 1 + 1);
	bad_str[next_size + 1] = '\0';
	for (uint i = 0; i <= next_size; i++) bad_str[i] = 'x';

	strbuf_printf(&buf, 0, bad_str);
	ASSERT_EQUAL(buf.size, strlen(bad_str));

	old_size = buf.size;

	strbuf_append(&buf, "x");
	ASSERT_EQUAL(buf.size, STRBUF_GROWTH_RATE * old_size);

	free(bad_str);
	strbuf_free(&buf);
}


void
strbuf_tests(void) {
	TEST_HEADER();

	strbuf_test_init();
	strbuf_test_printf();
	strbuf_test_append();
	strbuf_test_invisible();
	strbuf_test_resize();
	strbuf_test_growing();
}
