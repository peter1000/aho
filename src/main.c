void strbuf_tests(void);
void hash_tests(void);
void aho_tests(void);

int
main(void) {
	printf("Running all tests...\n");
	strbuf_tests();
	hash_tests();
	aho_tests();

	return (0);
}
