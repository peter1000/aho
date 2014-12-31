/*
 * aho.c: Aho-Corasick string matcher
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL.
 *
 * Created by David Čepelík <david@cepelik.eu> in 2014.
 */

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
 * Implements an AVL tree to hold node's children (to hold a big alphabet).
 */
#define TREE_ITEM       struct node
#define TREE_KEY        c
#define TREE_KEY_TYPE   uint
#define TREE_PREFIX(x)  avl_##x
#include "avl.c"


struct list lst;


static struct node *
node_new() {
	struct node *n = malloc_safe(sizeof(struct node));

	n->children = malloc_safe(sizeof(avl_tree)); // TODO static alloc
	avl_init(n->children);

	n->parent = NULL;
	n->back = NULL;
	n->back_out = NULL;
	n->c = 0;
	n->out = false;

	return (n);
}


void
aho_init(struct dict *d) {
	d->changed = true; // rebuilds empty dict

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
		n = avl_search(n->children, c);

		if (n != NULL) AHO_DEBUG(AHO_DEBUG_NODE_FORMAT, c);
	}

	AHO_DEBUG("%s", "]");

	if (c == '\0') {
		if (prev != NULL) {
			AHO_DEBUG(" ... Path already exists.\n");

			n->out = true;
			n->sample = sample;
			n->sample_strlen = strlen(sample);
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
		avl_insert(prev->children, n);
		prev = n;

#ifdef  AHO_DEBUG
		strbuf_append(&buf, "%c", n->c);
		n->prefix = strbuf_copy(&buf); // copy the prefix into the node
#endif
	}
	while ((c = sample[i++]) != '\0');

	prev->out = true; // sample ends here
	prev->sample = sample;
	prev->sample_strlen = strlen(sample);

	AHO_DEBUG(" ... Done.\n");

#ifdef  AHO_DEBUG
	strbuf_free(&buf);
#endif

	return (0);
}


void
append_child(struct node *n) {
	list_append(&lst, n);
}

static void
rebuild(struct dict *d) {
	AHO_DEBUG("%s", "Calculating the transition and output functions...\n");

	list_init(&lst);

	d->root->back = d->root;
	list_append(&lst, d->root);


	struct node *n;
	while ((n = list_shift(&lst)) != NULL) {
		// enqueue work
		avl_each(n->children, n->children->root, &append_child);

		n->back = n->parent->back;
		while (true) {
			if (n->parent == d->root) break;
			if (avl_search(n->back->children, n->c) != NULL) {
				n->back = avl_search(n->back->children, n->c);
				break;
			}

			if (n->back == d->root) break;
			n->back = n->back->back;
		}

		// now that we have n->back, we can compute n->back_out

		n->back_out = n->back;
		while (n->back_out != d->root) {
			if (n->back_out->out) {
				break;
			}

			n->back_out = n->back_out->back;
		}

		AHO_DEBUG("\tS('%s') := '%s', ", n->prefix, n->back->prefix);
		AHO_DEBUG("\tB('%s') := '%s'\n", n->prefix, n->back_out->prefix);
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
			s->back_node = s->back_node->back_out;
		} while (!s->back_node->out && s->back_node != d->root);

		if (s->back_node->out) {
			m->sample = s->back_node->sample;
			m->offset = i - s->back_node->sample_strlen;

			return (0);
		}
	}

	unsigned char c;
	while ((c = s->str[i++]) != '\0') {
		if (avl_search(n->children, c)) {
			n = avl_search(n->children, c);
			AHO_DEBUG("Have '%s'\n", n->prefix);
		} else {
			do {
				n = n->back;
				if (avl_search(n->children, c)) {
					n = avl_search(n->children, c);
					break;
				}
			} while (n != d->root);

			AHO_DEBUG("Missing '%c', transition to '%s'\n", c, n->prefix);
		}

		if (n->out) {
			s->cur_node = s->back_node = n;
			s->offset = i;

			m->sample = n->sample;
			m->offset = i - n->sample_strlen;

			return (0);
		}

		if (n->back_out != d->root) {
			s->cur_node = n;
			s->back_node = n->back_out;
			s->offset = i;

			m->sample = n->back_out->sample;
			m->offset = i - n->back_out->sample_strlen;

			return (0);
		}
	}

	return (-1);
}


void
node_free(struct node *n) {
	avl_each(n->children, n->children->root, &node_free);

	avl_free(n->children);
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
