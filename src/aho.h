/*
 * aho.h: Aho-Corasick string matcher
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL.
 *
 * Created by David Čepelík <david@cepelik.eu> in 2014.
 */

#ifndef AHO_H
#define AHO_H

#include "common.h"


#ifndef AHO_DEBUG
#define AHO_DEBUG(...)
#else
#undef  AHO_DEBUG
#define AHO_DEBUG(...) DEBUG(__VA_ARGS__)
#endif


#define AHO_DEBUG_NODE_FORMAT "%c"


#define AHO_INIT_HASH_SIZE 3


/*
 * Trie node.
 */
struct node {
	struct node *parent, *back;
	struct hash *children; // generic hashtable (see aho.c)
	uint c;
	char *sample; 
	bool out;

#ifdef  AHO_DEBUG
	char *prefix;
#endif
};


/*
 * Sample dictionary.
 */
struct dict {
	struct node *root;
	bool changed;
};


/*
 * FSM state encapsulation.
 */
struct state {
	char *str;
	struct node *cur_node, *back_node; // node for backtracking
	uint offset;
};


/*
 * A search match.
 */
struct match {
	char *sample;
	uint offset;
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
 * Reset FSM state.
 */
void aho_reset(struct dict *d, struct state *s, char *str);


/*
 * Get next matched sample.
 */
int aho_next(struct dict *d, struct state *s, struct match *m);


/*
 * Free memory used by the dictionary.
 */
void aho_free(struct dict *d);


#endif
