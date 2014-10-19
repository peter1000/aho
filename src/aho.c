#include <stdlib.h>

#include "aho.h"
#include "strbuf.h"


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
	d->root->prefix = DEBUG_EMPTY_STR;

	d->root->parent = d->root;
	d->changed = false;
}


int
aho_insert(struct dict *d, char *sample) {
	DEBUG("Inserting '%s' into the dictionary: [%s", sample, DEBUG_EMPTY_STR);
	d->changed = true; // TODO

	struct node *prev = NULL, *n = d->root;
	unsigned i = 0;
	unsigned char c;
	while (n != NULL && (c = sample[i++]) != '\0') {
		prev = n;
		n = n->children[c];

		if (n) DEBUG(DEBUG_NODE_FORMAT, c);
	}

	DEBUG("%s", "]");

	if (c == '\0') {
		if (prev != NULL) {
			DEBUG(" ... Path already present.\n");

			n->out = true;
			return (0);
		}

		// error, cannot insert the empty string
		return (1);
	}

	// extending the trie from `prev` node and `c` is the next char

	strbuf buf;
	strbuf_init(&buf);
	strbuf_append(&buf, "%s", prev->prefix);

	do {
		DEBUG(DEBUG_NODE_FORMAT, c);

		struct node *n = node_new();
		n->parent = prev;
		n->c = c;
		prev = prev->children[c] = n;

		strbuf_append(&buf, "%c", n->c);
		n->prefix = strbuf_copy(&buf);
	}
	while ((c = sample[i++]) != '\0');

	prev->out = true;

	DEBUG(" ... Done.\n");
	strbuf_free(&buf);
	return (0);
}


static void
rebuild(struct dict *d) {
	DEBUG("%s", "Rebuilding dictionary ...\n");

	struct list lst;
	list_init(&lst);

	d->root->back = d->root;
	list_append(&lst, d->root);

	struct node *n;
	while ((n = list_shift(&lst)) != NULL) {
		// enqueue work

		for (unsigned i = 0; i < 256; i++) { // TODO
			if (n->children[i] != NULL) {
				list_append(&lst, n->children[i]);
			}
		}

		n->back = n->parent->back;
		while (true) {
			if (n->parent == d->root) break;

			if (n->back->children[n->c] != NULL) {
				n->back = n->back->children[n->c];
				break;
			}

			if (n->back == d->root) break;
			n->back = n->back->back;
		}

		DEBUG("\tS('%s') := '%s'\n", n->prefix, n->back->prefix);
	}

	list_free(&lst);

	DEBUG("Done rebuilding dictionary.\n");
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
		if (n->children[c] != NULL) {
			n = n->children[c];
			DEBUG("Have '%s'\n", n->prefix);
		} else {
			do {
				n = n->back;
				if (n->children[c] != NULL) {
					n = n->children[c];
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

}
