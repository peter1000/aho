/*
 * main.c: Test runner
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL.
 *
 * Created by David Čepelík <david@cepelik.eu> in 2014.
 */

#include "common.h"
#include "test.h"
#include "aho.h"


void strbuf_tests(void);
void hash_tests(void);
void aho_tests(void);


void
aho_test_adhoc(void) {
	TEST_HEADER();

	char *samples[] = {
		"vzorek",
		"vzorecek",
		"vzor",
		"rek",
		"receklace", // :) 
		"rektor",
		"korek",
		"korektur"
	};

	char *str = "vzorek korektur rektoru naznacil, jak rekl rek, ze vzorecek z radikalu nese stopy receklace materialu";

	// 1. define and initialize a dictionary
	struct dict d;
	aho_init(&d);

	// 2. insert samples into the dictionary
	for (uint i = 0; i < ARRAY_SIZE(samples); i++) {
		aho_insert(&d, samples[i]);
	}

	// 3. reset internal FSM
	struct state s;
	aho_reset(&d, &s, str);

	// 4. find all occurences of all samples
	struct match m;

	while (aho_next(&d, &s, &m) != -1) {
		printf("- Found %s at offset %i\n", m.sample, m.offset);
	}

	// 5. everyone shall be free
	aho_free(&d);
}


int
main(void) {
	printf("Running all tests...\n");
	strbuf_tests();
	hash_tests();
	aho_tests();

	aho_test_adhoc();

	printf("\nDone.\n");

	return (0);
}
