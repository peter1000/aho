#include "../test.h"


struct item {
	uint key, val;
	uint check;
};

#define ITEM struct item *
#define KEY(item) item->key
#include "hashtable.inc"


#define HASH_TEST_INIT_SIZE 4
#define HASH_TEST_ITEM_COUNT 4


struct hash h;
struct item items[HASH_TEST_ITEM_COUNT];


static void
insert_all(void) {
	for (uint i = 0; i < HASH_TEST_ITEM_COUNT; i++) {
		hash_insert(&h, &items[i]);
	}
}


void
hash_test_init(void) {
	hash_init(&h, HASH_TEST_INIT_SIZE);
	
	ASSERT_EQUAL(h.count, 0);
	ASSERT_EQUAL(h.size, HASH_TEST_INIT_SIZE);
	ASSERT_NOT_NULL(h.items);

	hash_free(&h);
}


void
hash_test_insert_and_contains(void) {
	hash_init(&h, HASH_TEST_ITEM_COUNT);
	insert_all();

	for (uint i = 0; i < HASH_TEST_ITEM_COUNT; i++) {
		ASSERT_EQUAL(hash_find(&h, items[i].key), &items[i]);
		ASSERT_TRUE(hash_contains(&h, items[i].key));
	}

	hash_free(&h);
}


void
hash_test_growing(void) {
	hash_init(&h, HASH_TEST_ITEM_COUNT);
	insert_all();

	if (HASH_MAX_LOAD < 1) {
		ASSERT(h.size > HASH_TEST_ITEM_COUNT);
	}

	hash_free(&h);
}


void
check(struct item *i) {
	i->check = 1;
}


void
hash_test_each(void) {
	hash_init(&h, 2 * HASH_TEST_ITEM_COUNT);
	insert_all();

	hash_each(&h, &check);

	for (uint i = 0; i < HASH_TEST_ITEM_COUNT; i++) {
		ASSERT_TRUE(items[i].check);
	}

	hash_free(&h);
}


void
hash_tests(void) {
	TEST_HEADER();

	for (uint i = 0; i < HASH_TEST_ITEM_COUNT; i++) {
		items[i].key = items[i].val = i;
		items[i].check = 0;
	}

	hash_test_init();
	hash_test_insert_and_contains();
	hash_test_growing();
	hash_test_each();
}
