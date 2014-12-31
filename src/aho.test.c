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


struct dict d;


void
aho_test_init(void) {
	aho_init(&d);

	ASSERT_NOT_NULL(d.root);
	ASSERT_TRUE(d.changed);

	aho_free(&d);
}


void
insert_samples(char *samples[]) {
	for (uint i = 0; i < ARRAY_SIZE(samples); i++) {
		aho_insert(&d, samples[i]);
	}
}


void
aho_test_insert_and_search(void) {
	aho_init(&d);

	char *samples[] = {
		"over",
		"rover",
		"ove",
		"ver",
		"very",
		"version",
		"trie",
		"trick",
		"neck",
		"ale",
		"aleda",
		"led",
		"leda",
		"dakar"
	};

	for (uint i = 0; i < ARRAY_SIZE(samples); i++) {
		aho_insert(&d, samples[i]);
	}

	char *str = "roversioneckaledakar";

	struct state s;
	struct match m;


	// match is claimed at x => sample is at x

	aho_reset(&d, &s, str);
	while (aho_next(&d, &s, &m) != -1) {
		ASSERT_EQUAL(strstr(str, m.sample) - str, m.offset);
	}


	// sample is at x => match is found at x (if not covered by other sample)

	char *should_find[] = {
		"over",
		"ove",
		"rover",
		"ver",
		"version",
		"neck",
		"ale",
		"aleda",
		"led",
		"leda",
		"dakar"
	};

	for (uint i = 0; i < ARRAY_SIZE(should_find); i++) {
		bool found = false;
		aho_reset(&d, &s, str);
		while (aho_next(&d, &s, &m) != -1) {
			if (strcmp(m.sample, should_find[i]) == 0) {
				ASSERT_EQUAL(m.offset, strstr(str, m.sample) - str);
				found = true;
				break;
			}
		}

		if (!found) FAIL("sample '%s' will be found", should_find[i]);
	}

	aho_free(&d);
}


void
aho_test_search_empty_dict() {
	aho_init(&d);

	struct state s;
	aho_reset(&d, &s, "abcd");
	struct match m;

	ASSERT_EQUAL(aho_next(&d, &s, &m), -1);

	aho_free(&d);
}


void
aho_test_search_empty_string() {
	aho_init(&d);

	char *samples[] = {
		"a",
		"b",
		"c",
		"d",
	};
	insert_samples(samples);

	struct state s;
	aho_reset(&d, &s, "");

	struct match m;
	ASSERT_EQUAL(aho_next(&d, &s, &m), -1);

	aho_free(&d);
}


void
aho_test_search() {
	aho_init(&d);

	char *samples[] = {
		"a"
	};
	insert_samples(samples);

	ASSERT_TRUE(d.changed);

	char *str = "aaaaaa";

	struct state s;
	aho_reset(&d, &s, str);

	struct match m;
	for (uint i = 0; i < strlen(str); i++) {
		ASSERT_EQUAL(aho_next(&d, &s, &m), 0);
		ASSERT_MATCH(m.sample, samples[0]);
		ASSERT_EQUAL(m.offset, i);
	}

	ASSERT_FALSE(d.changed);

	aho_free(&d);
}


void
aho_tests(void) {
	TEST_HEADER();

	aho_test_init();
	aho_test_insert_and_search();
	aho_test_search_empty_dict();
	aho_test_search_empty_string();
	aho_test_search();
}
