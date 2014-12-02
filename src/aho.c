#include <string.h>

#include "aho.h"
#include "strbuf.h"
#include "common.h"


/*
 * Implements list of nodes (used as a simple queue).
 */
#define NODE struct node *
#include "generic/list.inc"


/*
 * Implements hashtable to hold node's children (allows for Unicode support).
 */
#define ITEM struct node *
#define KEY(node) node->c
#include "generic/hashtable.inc"


static struct node *
node_new() {
	struct node *n = malloc_safe(sizeof(struct node));

	n->children = malloc_safe(sizeof(struct hash)); // TODO static alloc
	hash_init(n->children, 3);

	n->parent = NULL;
	n->back = NULL;
	n->c = 0;
	n->out = false;

	return (n);
}


void
aho_init(struct dict *d) {
	d->changed = false;

	d->root = node_new();
	d->root->parent = d->root;

#ifdef  AHO_DEBUG
	d->root->prefix = NULL;
#endif
}


int
aho_insert(struct dict *d, char *sample) {
	AHO_DEBUG("Inserting '%s' into the dictionary: [", sample);
	d->changed = true;

	// find path corresponding to some prefix of the sample if it exists

	struct node *prev = NULL, *n = d->root;
	uint i = 0;
	unsigned char c;
	while (n != NULL && (c = sample[i++]) != '\0') {
		prev = n;
		n = hash_find(n->children, c);

		if (n != NULL) AHO_DEBUG(AHO_DEBUG_NODE_FORMAT, c);
	}

	AHO_DEBUG("%s", "]");

	if (c == '\0') {
		if (prev != NULL) {
			AHO_DEBUG(" ... Path already exists.\n");

			n->out = true;
			return (0);
		}

		AHO_DEBUG("Cannot insert empty string.\n");
		return (1);
	}

	// extending the trie (starting with `prev` node and `c` is next char)

#ifdef  AHO_DEBUG
	strbuf buf;
	strbuf_init(&buf);
	if (prev->prefix != NULL) strbuf_append(&buf, "%s", prev->prefix);
#endif

	do {
		AHO_DEBUG(AHO_DEBUG_NODE_FORMAT, c);

		struct node *n = node_new();
		n->parent = prev;
		n->c = c;
		hash_insert(prev->children, n);
		prev = n;

#ifdef  AHO_DEBUG
		strbuf_append(&buf, "%c", n->c);
		n->prefix = strbuf_copy(&buf); // copy the prefix into the node
#endif
	}
	while ((c = sample[i++]) != '\0');

	prev->out = true; // sample ends here
	prev->sample = sample;

	AHO_DEBUG(" ... Done.\n");

#ifdef  AHO_DEBUG
	strbuf_free(&buf);
#endif

	return (0);
}


static void
rebuild(struct dict *d) {
	AHO_DEBUG("%s", "Calculating the transition function S...\n");

	struct list lst;
	list_init(&lst);

	d->root->back = d->root;
	list_append(&lst, d->root);

	struct node *n;
	while ((n = list_shift(&lst)) != NULL) {
		// enqueue work

		for (uint i = 0; i < 256; i++) { // TODO Unicode
			if (hash_contains(n->children, i)) {
				list_append(&lst, hash_find(n->children, i));
			}
		}

		n->back = n->parent->back;
		while (true) {
			if (n->parent == d->root) break;
			if (hash_contains(n->back->children, n->c)) {
				n->back = hash_find(n->back->children, n->c);
				break;
			}

			if (n->back == d->root) break;
			n->back = n->back->back;
		}

		AHO_DEBUG("\tS('%s') := '%s'\n", n->prefix, n->back->prefix);
	}

	list_free(&lst);

	AHO_DEBUG("Done.\n");
}


void
aho_reset(struct dict *d, struct state *s, char *str) {
	s->str = str;
	s->cur_node = d->root;
	s->offset = 0;
	s->back_node = d->root;
}


int
aho_next(struct dict *d, struct state *s, struct match *m) {
	if (d->changed) {
		rebuild(d);
		d->changed = false;

		// added new samples, start over (prevents undefined behaviour)
		aho_reset(d, s, s->str);
	}

	uint i = s->offset;
	struct node *n = s->cur_node;

	if (s->back_node != d->root) {
		do {
			s->back_node = s->back_node->back;
		} while (!s->back_node->out && s->back_node != d->root);
		
		if (s->back_node->out) {
			m->sample = s->back_node->sample;
			m->offset = i - strlen(m->sample);

			return (0);
		}
	}


	unsigned char c;
	while ((c = s->str[i++]) != '\0') {
		if (hash_contains(n->children, c)) {
			n = hash_find(n->children, c);
			AHO_DEBUG("Have '%s'\n", n->prefix);
		} else {
			do {
				n = n->back;
				if (hash_contains(n->children, c)) {
					n = hash_find(n->children, c);
					break;
				}
			} while (n != d->root);

			AHO_DEBUG("Missing '%c', transition to '%s'\n", c, n->prefix);
		}

		if (n->out) {
			s->cur_node = s->back_node = n;
			s->offset = i;

			m->sample = n->sample;
			m->offset = i - strlen(m->sample);

			return (0);
		}
	}

	return (-1);
}


void
node_free(struct node *n) {
	hash_each(n->children, &node_free);

	// free memory used by the hash vs. free struct hash (2 hours to debug)
	hash_free(n->children);
	free(n->children);

#ifdef  AHO_DEBUG
	if (n->prefix != NULL) { // prevents segvault on the root
		free(n->prefix);
	}
#endif

	free(n);
}


void
aho_free(struct dict *d) {
	node_free(d->root);
}
