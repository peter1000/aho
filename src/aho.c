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

	n->children = malloc_safe(sizeof(struct hash)); // TODO why dynamic
	hash_init(n->children, 3);

	n->parent = NULL;
	n->back = NULL;
	n->c = 0;
	n->out = false;

	return (n);
}


void
aho_init(struct dict *d) {
	d->root = node_new();
	d->root->prefix = DEBUG_EMPTY_STR;

	d->root->parent = d->root;
	d->changed = false;
}


int
aho_insert(struct dict *d, char *sample) {
	DEBUG("Inserting '%s' into the dictionary: [%s", sample, DEBUG_EMPTY_STR);
	d->changed = true;

	// find path corresponding to some prefix of the sample if it exists

	struct node *prev = NULL, *n = d->root;
	unsigned i = 0;
	unsigned char c;
	while (n != NULL && (c = sample[i++]) != '\0') {
		prev = n;
		n = hash_find(n->children, c);

		if (n) DEBUG(DEBUG_NODE_FORMAT, c);
	}

	DEBUG("%s", "]");

	if (c == '\0') {
		if (prev != NULL) {
			DEBUG(" ... Path already exists.\n");

			n->out = true;
			return (0);
		}

		DEBUG("Cannot insert empty string.\n");
		return (1);
	}

	// extending the trie (starting with `prev` node and `c` is next char)

	strbuf buf;
	strbuf_init(&buf);
	strbuf_append(&buf, "%s", prev->prefix);

	do {
		DEBUG(DEBUG_NODE_FORMAT, c);

		struct node *n = node_new();
		n->parent = prev;
		n->c = c;
		prev = hash_insert(prev->children, n);

		strbuf_append(&buf, "%c", n->c);
		n->prefix = strbuf_copy(&buf); // copy the prefix into the node
	}
	while ((c = sample[i++]) != '\0');

	prev->out = true; // some sample ends here

	DEBUG(" ... Done.\n");
	strbuf_free(&buf);
	return (0);
}


static void
rebuild(struct dict *d) {
	DEBUG("%s", "Calculating the transition function S...\n");

	struct list lst;
	list_init(&lst);

	d->root->back = d->root;
	list_append(&lst, d->root);

	struct node *n;
	while ((n = list_shift(&lst)) != NULL) {
		// enqueue work

		for (unsigned i = 0; i < 256; i++) { // TODO oh not this
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

		DEBUG("\tS('%s') := '%s'\n", n->prefix, n->back->prefix);
	}

	list_free(&lst);

	DEBUG("Done.\n");
}


struct match *
aho_next(struct dict *d, char *str, struct match *m) {
	if (d->changed) {
		rebuild(d);
		d->changed = false;
	}

	struct node *n = d->root;
	unsigned i = 0;
	unsigned char c;
	while ((c = str[i++]) != '\0') {
		if (hash_contains(n->children, c)) {
			n = hash_find(n->children, c);
			DEBUG("Have '%s'\n", n->prefix);
		} else {
			do {
				n = n->back;
				if (hash_contains(n->children, c)) {
					n = hash_find(n->children, c);
					break;
				}
			} while (n != d->root);

			DEBUG("Missing '%c', transition to '%s'\n", c, n->prefix);
		}

		if (n->out) {
			struct node *m = n;
			while (m != d->root) {
				if (m->out) DEBUG("Found '%s'\n", m->prefix);
				m = m->back;
			}
		}
	}

	return (NULL);
}


void
aho_free(struct dict *d) {
	// TODO
}
