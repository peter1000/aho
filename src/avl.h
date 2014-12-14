/*
 *  trees/avl.h: Generic AVL trees
 *
 *  This software may be freely used and distributed according to the terms
 *  of the GNU GPL.
 *
 *  Created by David Čepelík <david@cepelik.eu> in 2014.
 */

#define P(x)  TREE_PREFIX(x)


typedef struct P(node) {
	struct P(node) *p, *left, *right;
	TREE_ITEM *item;
	unsigned int height;
} P(node);


typedef struct {
	P(node) *root, *nil;
	unsigned int count;
} P(tree);


void
P(init)(P(tree) *t);


void
P(insert)(P(tree) *t, TREE_ITEM *item);


void
P(delete)(P(tree) *t, TREE_KEY_TYPE key);


TREE_ITEM *
P(search)(P(tree) *t, TREE_KEY_TYPE key);

void
P(each)(P(tree) *t, P(node) *n, void (*f)(TREE_ITEM *));


char *
P(tree_dump)(P(tree) *t);


void
P(free)(P(tree) *t);


#undef P

#undef TREE_ITEM
#undef TREE_KEY
#undef TREE_KEY_TYPE
#undef TREE_PREFIX
#undef TREE_COMPARE
