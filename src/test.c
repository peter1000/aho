#include <stdio.h>
#include <stdarg.h>
#include "strbuf.h"


void
fail(char *file, unsigned int line, char *format, ...) {
	va_list args;
	va_start(args, format);

	strbuf buf;
	strbuf_init(&buf);
	strbuf_vprintf_at(&buf, 0, format, args);
	va_end(args);

	printf("Assertion failed: %s\n\t\tin %s on line %i\n", buf.str, file, line);
}


void
pass(void) {
	printf(".");
}
