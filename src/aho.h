#ifndef AHO_H_
#define AHO_H_

#include "common.h"


#define DEBUG_NODE_FORMAT "%c"
#define DEBUG_EMPTY_STR "*"


/*
 * Trie node.
 */
struct node {
	struct node *parent, *back;
	struct hash *children; // generic hashtable (see aho.c)
	uint c;
	bool out;
	char *prefix;
};


/*
 * Sample dictionary.
 */
struct dict {
	struct node *root;
	bool changed;
};


/*
 * A search match.
 */
struct match {
	char *sample;
	unsigned offset;
};


/*
 * Initialize new dictionary.
 */
void aho_init(struct dict *d);


/*
 * Insert a sample into the dictionary.
 */
int aho_insert(struct dict *d, char *sample);


/*
 * Get next matched sample.
 */
struct match *aho_next(struct dict *d, char *str, struct match *m);


/*
 * Free memory used by the dictionary.
 */
void aho_free(struct dict *d);


#endif
