/*
 *  trees/avl.c: Generic AVL trees
 *
 *  See the header file for information about intended usage!
 *
 *  This software may be freely used and distributed according to the terms
 *  of the GNU GPL.
 *
 *  Created by David Čepelík <david@cepelik.eu> in 2014.
 */

#include "strbuf.h"


#ifndef TREE_ITEM
#error Please define the TREE_ITEM macro.
#endif

#ifndef TREE_KEY
#error Please define the TREE_KEY macro.
#endif

#ifndef TREE_KEY_TYPE
#error Please define the TREE_KEY_TYPE macro.
#endif

#ifndef TREE_PREFIX
#error Please define the TREE_PREFIX macro.
#endif


#define P(x)  TREE_PREFIX(x)


#ifndef TREE_COMPARE
#define TREE_COMPARE(x, y)  ((x) < (y) ? -1 : ((x) == (y) ? 0 : 1))
#endif

/*
 *  Default printf() modifier used to print the key.
 */
#ifndef TREE_KEY_PRINTF
#define TREE_KEY_PRINTF "%i"
#endif

#define KEY(item)  (item->TREE_KEY)


static P(node) *P(node_new)(void) {
	P(node) *n = malloc_safe(sizeof(P(node)));
	n->p = n->left = n->right = NULL;
	n->height = 0;
	n->item = NULL;

	return n;
}


void P(init)(P(tree) *t) {
	t->nil = P(node_new)();
	t->nil->height = 0;
	t->root = t->nil;
	t->count = 0;
}


static inline void rotate_left(P(tree) *t, P(node) *x) { // see ITA, pp. 277-279
	P(node) *y = x->right;

	x->right = y->left;
	if (y->left != t->nil)
		y->left->p = x;

	y->p = x->p;
	if (x->p != t->nil) {
		if (x->p->left == x) x->p->left = y;
		else x->p->right = y;
	}
	else t->root = y;

	y->left = x;
	x->p = y;

	x->height = MAX(x->left->height, x->right->height) + 1;
	y->height = MAX(x->height, y->right->height) + 1;
}


static inline void rotate_right(P(tree) *t, P(node) *y) {
	P(node) *x = y->left;

	y->left = x->right;
	if (x->right != t->nil)
		x->right->p = y;

	x->p = y->p;
	if (y->p != t->nil) {
		if (y->p->right == y) y->p->right = x;
		else y->p->left = x;
	}
	else t->root = x;

	x->right = y;
	y->p = x;

	y->height = MAX(y->left->height, y->right->height) + 1;
	x->height = MAX(x->left->height, y->height) + 1;
}


static P(node) *search(P(tree) *t, TREE_KEY_TYPE key) {
	P(node) *x = t->root;

	while (x != t->nil && TREE_COMPARE(key, KEY(x->item)) != 0) {
		if (TREE_COMPARE(key, KEY(x->item)) < 0) x = x->left;
		else x = x->right;
	}

	return x != t->nil ? x : NULL;
}


TREE_ITEM *P(search)(P(tree) *t, TREE_KEY_TYPE key) {
	P(node) *n = search(t, key);
	if (n) return n->item;
	return NULL;
}


static P(node) *min(P(tree) *t, P(node) *x) {
	while (x->left != t->nil) x = x->left;
	return x;
}


static P(node) *successor(P(tree) *t, P(node) *x) {
	if (x->right != t->nil) return min(t, x->right);
	P(node) *y = x->p;

	while (y != t->nil && x == y->right) {
		x = y;
		y = y->p;
	}
	return y;
}


static int balance(P(node) *n) {
	return n->right->height - n->left->height;
}


static void insert_fixup(P(tree) *t, P(node) *n) {
	while (n != t->root) {
		if (balance(n->p) == 0) {
			n->height = MAX(n->left->height, n->right->height) + 1;
			n = n->p;
		}
		else {
			if (n == n->p->left) {
				if (balance(n->p) == -1) {
					if (balance(n) == -1) {
						rotate_right(t, n->p); // fixed
					}
					else if (balance(n) == 1) {
						rotate_left(t, n);
						rotate_right(t, n->p->p); // fixed
					}
				}
			}
			else {
				if (balance(n->p) == 1) {
					if (balance(n) == 1) {
						rotate_left(t, n->p); // fixed
					}
					else if (balance(n) == -1) {
						rotate_right(t, n);
						rotate_left(t, n->p->p); // fixed
					}
				}
			}

			n->height = MAX(n->left->height, n->right->height) + 1;
			n = t->root; // stops iteration
		}
	}
}


void P(insert)(P(tree) *t, TREE_ITEM *item) {
	P(node) *y = t->nil, *x = t->root;
	while (x != t->nil) {
		y = x;

		if (TREE_COMPARE(KEY(item), KEY(x->item)) < 0) x = x->left;
		else x = x->right;
	}

	P(node) *n = P(node_new)();
	n->item = item;

	n->p = y;
	if (y != t->nil) {
		if (TREE_COMPARE(KEY(item), KEY(y->item)) < 0) y->left = n;
		else y->right = n;
	}
	else t->root = n;

	n->left = n->right = t->nil;

	t->count++;
	insert_fixup(t, n);
}


static void delete_fixup(P(tree) *t, P(node) *n) {
	P(node) *prev = t->nil, *n0 = n;
	int sibling_b, parent_b; // balance factors

	while (n != t->root) {
		if (prev != t->nil) prev->height = MAX(prev->left->height, prev->right->height) + 1;

		if (prev == n0) prev->height = MAX(prev->left->height, prev->right->height);
		prev = n;

		parent_b = balance(n->p);

		if (parent_b == 0) break;

		if (n == n->p->left) {
			if (parent_b == -1) {
				n = n->p;
				continue;
			}

			P(node) *s = n->p->right;
			sibling_b = balance(s);

			if (sibling_b == -1) rotate_right(t, s);

			rotate_left(t, n->p);
			if (sibling_b == 0) break;
			n = n->p;
		}
		else {
			if (parent_b == 1) {
				n = n->p;
				continue;
			}

			P(node) *s = n->p->left;
			sibling_b = balance(s);

			if (sibling_b == 1) rotate_left(t, s);

			rotate_right(t, n->p);
			if (sibling_b == 0) break;
			n = n->p;
		}
	}

	if (prev != t->nil) {
		prev->height = MAX(prev->left->height, prev->right->height) + 1;
	}
	
	// this may leave root's height inconsistent, but that does not matter:
	// root node's balance is depends only on children's heights and, when
	// rotated, heights are recalculated
}


void P(delete)(P(tree) *t, TREE_KEY_TYPE key) {
	P(node) *z = search(t, key);
	if (!z) return;

	// z is the node we want to delete, y the node we will actually delete and
	// x is the node to take its place
	P(node) *y, *x;

	if (z->left == t->nil || z->right == t->nil) y = z;
	else y = successor(t, z);

	delete_fixup(t, y);

	if (y->left != t->nil) x = y->left;
	else x = y->right;

	if (y != z) z->item = y->item;

	// replace y by x
	x->p = y->p;
	if (y->p == t->nil) t->root = x;
	else {
		if (y == y->p->left) y->p->left = x;
		else y->p->right = x;
	}

	t->count--;
	// TODO deallocate
}


static void dump_subtree(P(tree) *t, P(node) *n, strbuf *buf) {
	if (n == t->nil) {
		strbuf_append(buf, "* ");
	}
	else {
		strbuf_append(buf, TREE_KEY_PRINTF"(%i) ", KEY(n->item), balance(n));
		dump_subtree(t, n->left, buf);
		dump_subtree(t, n->right, buf);
	}
}


char *P(tree_dump)(P(tree) *t) {
	strbuf buf;
	strbuf_init(&buf);
	dump_subtree(t, t->root, &buf);
	buf.str[buf.offset - 1] = '\0'; // trim trailing space
	return buf.str;
}


void P(each)(P(tree) *t, P(node) *n, void (*f)(TREE_ITEM *))  {
	if (n == t->nil) return;

	if (n->left != t->nil) P(each)(t, n->left, f);
	if (n->right != t->nil) P(each)(t, n->right, f);

	f(n->item);
}


static void P(node_free)(P(tree) *t, P(node) *n) {
	if (n == t->nil) return;

	P(node_free)(t, n->left);
	P(node_free)(t, n->right);

	free(n);
}


void P(free)(P(tree) *t) {
	P(node_free)(t, t->root);
	free(t->nil);
}


#undef P

#undef TREE_ITEM
#undef TREE_KEY
#undef TREE_KEY_TYPE
#undef TREE_PREFIX
#undef TREE_COMPARE
