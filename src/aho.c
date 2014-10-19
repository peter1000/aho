#include <stdlib.h>

#include "aho.h"


#define NODE struct node *
#include "generic/list.inc"


static struct node *
node_new() {
	struct node *n = malloc(sizeof(struct node));
	// error if n == NULL

	n->children = calloc(256, sizeof(struct node *)); // TODO
	// error if n->children == NULL

	n->parent = NULL;
	n->back = NULL;
	n->c = 0;
	n->out = false;

	return (n);
}


void
aho_init(struct dict *d) {
	// error if d == NULL
	d->root = node_new();
	d->root->parent = d->root;
	d->changed = false;
}


int
aho_insert(struct dict *d, char *sample) {
	DEBUG("Inserting '%s' into the dictionary: [ *", sample);
	d->changed = true; // TODO

	struct node *prev = NULL, *n = d->root;
	unsigned i = 0;
	unsigned char c;
	while (n != NULL && (c = sample[i++]) != '\0') {
		prev = n;
		n = n->children[c];

		if (n) DEBUG(DEBUG_NODE, c);
	}

	DEBUG("%s", " ]");

	if (c == '\0') {
		if (prev != NULL) {
			DEBUG("\n\tNothing to add for '%s'\n", sample);
			prev->out = true;
			return (0);
		}

		// error, cannot insert the empty string
		return (1);
	}

	// extending the trie from `prev` node and `c` is the next char

	do {
		DEBUG(DEBUG_NODE, c);

		struct node *n = node_new();
		n->parent = prev;
		n->c = c;
		prev = prev->children[c] = n;
	}
	while ((c = sample[i++]) != '\0');

	DEBUG("\n\tDone adding '%s'\n", sample);

	return (0);
}


static void
rebuild(struct dict *d) {
	DEBUG("%s", "Rebuilding dictionary\n");

	struct list lst;
	list_init(&lst);

	d->root->back = d->root;
	list_append(&lst, d->root);

	struct node *n;
	while ((n = list_shift(&lst)) != NULL) {
		for (unsigned i = 0; i < 256; i++) { // TODO
			if (n->children[i] != NULL) {
				list_append(&lst, n->children[i]);
			}
		}

		struct node *back = n->parent->back;
		while (back != d->root) {
			if (back->children[n->c] != NULL) {
				back = back->children[n->c];
				break;
			}

			back = back->back;
		}

		n->back = back;

	}

	list_free(&lst);
}


struct match *
aho_next(struct dict *d, char *str, struct match *m) {
	if (d->changed) {
		rebuild(d);
		d->changed = false;
	}


	return (NULL);
}


void
aho_free(struct dict *d) {

}
