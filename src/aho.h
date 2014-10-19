#ifndef AHO_H_
#define AHO_H_

#include <stdio.h>
#include <stdbool.h>


#ifdef DEBUG
#define DEBUG(...) printf(__VA_ARGS__);
#else
#define DEBUG(...) 
#endif

#define DEBUG_NODE_FORMAT "%c"
#define DEBUG_EMPTY_STR "*"


struct node {
	struct node *parent;
	struct node **children;
	struct node *back;
	unsigned char c;
	bool out;
#ifdef DEBUG
	char *prefix;
#endif
};


struct dict {
	struct node *root;
	bool changed;
};


struct match {
	char *sample;
	unsigned offset;
};


void aho_init(struct dict *d);


int aho_insert(struct dict *d, char *sample);


struct match *aho_next(struct dict *d, char *str, struct match *m);


void aho_free(struct dict *d);


#endif
