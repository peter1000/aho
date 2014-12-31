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

#include <stdio.h>


void strbuf_tests(void);
void hash_tests(void);
void aho_tests(void);


void
aho_test_adhoc(void) {
	TEST_HEADER();

	FILE *f = fopen("aho-test.txt", "r");

	// 1. define and initialize a dictionary
	struct dict d;
	aho_init(&d);

	int sample_count;
	fscanf(f, "%i", &sample_count);

	// 2. insert samples into the dictionary
	for (uint i = 0; i < sample_count; i++) {
		char *sample = malloc(512);
		fscanf(f, "%s", sample);

		aho_insert(&d, sample);
	}

	char str[65535];
	fscanf(f, "%s", str);

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

	fclose(f);
}


int
main(void) {
	printf("Running all tests...\n");
	//aho_test_adhoc();

	strbuf_tests();
	hash_tests();
	aho_tests();

	printf("\nDone.\n");

	return (0);
}
