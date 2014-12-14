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
// TODO #define AHO_NOT_FOUND -1


#define TREE_ITEM       struct node
#define TREE_KEY        c
#define TREE_KEY_TYPE   uint
#define TREE_PREFIX(x)  avl_##x
#include "avl.h"


/*
 * Trie node.
 */
struct node {
	struct node *parent, *back, *back_out; // back_out: for output function
	avl_tree *children; // generic hashtable (see aho.c)
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
 * 
 * O(1) time
 */
void aho_init(struct dict *d);


/*
 * Insert a sample into the dictionary.
 *
 * O(n log(|\sigma|)) time, where n = strlen(sample):
 */
int aho_insert(struct dict *d, char *sample);


/*
 * Reset FSM state.
 *
 * O(1) time
 */
void aho_reset(struct dict *d, struct state *s, char *str);


/*
 * Get next matched sample.
 *
 * If the dictionary changed, the data structure needs to be rebuilt; this is
 * O(J log(|\sigma|)), where J = |size of all needles combined|.
 *
 * Otherwise, the time is O(1) per match, ie. O(n^2) when called for each match.
 *
 */
int aho_next(struct dict *d, struct state *s, struct match *m);


/*
 * Free memory used by the dictionary.
 *
 * O(mn) time where n is the number of samples and m is the lenght of the
 * longest sample. (I need to undo all allocs.)
 */
void aho_free(struct dict *d);


#endif
