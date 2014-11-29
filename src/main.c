#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "aho.h"


int
main(void) {
	//return (0);

	struct dict d;
	aho_init(&d);

	aho_insert(&d, "rover");
	aho_insert(&d, "over");
	aho_insert(&d, "ove");
	aho_insert(&d, "version");
	aho_insert(&d, "ver");
	aho_insert(&d, "very");
	aho_insert(&d, "trick");
	aho_insert(&d, "trie");

	aho_next(&d, "roversion", NULL);

	printf("Aho-Corasick done\n");

	return (0);
}
