
#include "../src/aho.h"


int
main(void) {
	struct dict d;
	aho_init(&d);

	aho_insert(&d, "rover");
	aho_insert(&d, "over");
	aho_insert(&d, "ove");
	aho_insert(&d, "version");
	aho_insert(&d, "very");
	aho_insert(&d, "trick");
	aho_insert(&d, "trie");

	aho_next(&d, "roverie", NULL);

	printf("Aho-Corasick done\n");

	return (0);
}
