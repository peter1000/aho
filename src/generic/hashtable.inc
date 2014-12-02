#include "../primes.h"
#include "../common.h"


#ifndef HASH_DEBUG
#define HASH_DEBUG(...)
#else
#undef 	HASH_DEBUG
#define HASH_DEBUG(...) DEBUG(__VA_ARGS__)
#endif


#ifndef PREFIX
#define PREFIX(x) x
#endif

#define P(x) PREFIX(x)


#ifndef ITEM
#error "Define ITEM macro prior to including " __FILE__
#endif

#ifndef KEY
#error "Define KEY macro prior to including " __FILE__
#endif


#define HASH_MAX_LOAD 0.55


struct P(hash) {
	ITEM *items;
	uint size, count;
};


static int
P(hash_init)(struct P(hash) *t, uint size) {
	if (size == 0) {
		HASH_DEBUG("Table size must be a positive number.\n");
		return (-1);
	}

	t->count = 0;
	t->size = size;

	t->items = calloc(t->size, sizeof(ITEM));
	if (t == NULL) return (-1);

	return (0);
}


static ITEM
P(hash_insert)(struct P(hash) *t, ITEM item);


static int
resize(struct P(hash) *t, uint new_size) {
	HASH_DEBUG("Resizing hashtable... ");

	ITEM *items = t->items; // "old" items (items in the original table)
	uint old_size = t->size;

	if (P(hash_init)(t, new_size) == -1) {
		free(items);
		return (-1);
	}

	for (uint i = 0; i < old_size; i++) {
		if (items[i] != NULL) P(hash_insert)(t, items[i]);
	}

	free(items);
	return (0);
}


static uint
hash(uint k, uint m) { // TODO
	return k % m;
}


static ITEM
P(hash_insert)(struct P(hash) *t, ITEM item) {
	if (t->count + 1 > t->size) {
		HASH_DEBUG("The hashtable is full. Is HASH_MAX_LOAD reasonable?\n");
		return NULL;
	}

	uint h = hash(KEY(item), t->size);

	// find empty slot (at least one exists)
	while (t->items[h] != NULL) {
		h = (h + 1) % t->size;
	}

	t->items[h] = item;
	t->count++;

	float load = (float)t->count / t->size;
	if (load > HASH_MAX_LOAD) {
		resize(t, some_prime_greater_than(2 * t->size)); // log cost
	}

	return item;
}


static uint // TODO bool
P(hash_contains)(struct P(hash) *t, uint key) {
	uint h = hash(key, t->size);

	while (t->items[h] != NULL) {
		if (KEY(t->items[h]) == key) return 1;
		h = (h + 1) % t->size;
	}

	return 0;
}


static ITEM
P(hash_find)(struct P(hash) *t, uint key) {
	uint h = hash(key, t->size);

	// the cycle prevents infinite loop when a miss occurs in a full table
	for (uint i = 0; i < t->size; i++) {
		if (t->items[h] == NULL || KEY(t->items[h]) == key) break;
		h = (h + 1) % t->size;
	}

	return (t->items[h]);
}


static void
P(hash_each)(struct P(hash) *t, void (*f)(ITEM)) {
	for (uint i = 0; i < t->size; i++) {
		if (t->items[i] == NULL) continue;
		f(t->items[i]);
	}
}


static void
P(hash_free)(struct P(hash) *t) {
	free(t->items);
}


#undef P

#undef PREFIX
#undef ITEM
#undef KEY
