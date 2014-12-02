/*
 * aho.test.c: Tests of the Aho-Corasick string matcher
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL.
 *
 * Created by David Čepelík <david@cepelik.eu> in 2014.
 */

#include <string.h>

#include "test.h"
#include "aho.h"


#define AHO_TEST_STR "roversioneckaledakar"


struct dict d;


void
aho_test_init(void) {
	aho_init(&d);
	ASSERT_NOT_NULL(d.root);
	ASSERT_FALSE(d.changed);

	aho_free(&d);
}


void
insert_samples(char *samples[]) {
	for (uint i = 0; i < ARRAY_SIZE(samples); i++) {
		aho_insert(&d, samples[i]);
	}
}


void
aho_test_search() {
	char *samples[] = {
		"a"
	};

	insert_samples(samples);
}


void
aho_test_insert_and_search(void) {
	aho_init(&d);

	char *samples[] = {
		"ove",
		"over",
		"rover",
		"ver",
		"very",
		"version",
		"trie",
		"trick",
		"neck",
		"ale",
		"aleda",
		"led",    // "led"  WILL NOT be found (covered by "aleda")
		"leda",   // "leda" WILL be found
		"dakar"
	};

	for (uint i = 0; i < ARRAY_SIZE(samples); i++) {
		aho_insert(&d, samples[i]);
	}

	char *str = AHO_TEST_STR;

	struct match m;
	struct state s;
	aho_reset(&d, &s, str);

	// match is claimed at x => sample is at x
	while (aho_next(&d, &s, &m) != -1) {
		ASSERT_EQUAL(strstr(str, m.sample) - str, m.offset);
	}

	// sample is at x => match is claimed at x

	aho_free(&d);
}


void
aho_tests(void) {
	TEST_HEADER();

	aho_test_init();
	aho_test_insert_and_search();
}
