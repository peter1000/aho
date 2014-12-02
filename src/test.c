#include "strbuf.h"
#include "common.h"


void
fail(char *file, uint line, char *format, ...) {
	va_list args;
	va_start(args, format);

	strbuf buf;
	strbuf_init(&buf);
	strbuf_vprintf_at(&buf, 0, format, args);
	va_end(args);

	printf("! Failed asserting that %s at %s:%i\n", buf.str, file, line);
	strbuf_free(&buf);
}


void
pass() {
	printf("+\n");
}
