/*
 * strbuf.c: Growing string buffer
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL.
 *
 * Created by David Čepelík <david@cepelik.eu> in 2014.
 */

#include <stdlib.h>
#include <stdio.h>

#include "strbuf.h"


void
strbuf_resize(strbuf *buf, size_t new_size) {
	buf->str = realloc_safe(buf->str, new_size + 1);

	buf->size = new_size;
	buf->offset = buf->offset < buf->size ? buf->offset : buf->size;
	buf->str[buf->offset] = '\0';
}


void
strbuf_init(strbuf *buf) {
	buf->offset = 0;
	buf->str = NULL;
	strbuf_resize(buf, STRBUF_INIT_SIZE);
}


void
strbuf_vprintf_at(strbuf *buf, size_t offset, char *format, va_list args) {
	va_list args2;
	va_copy(args2, args);

	size_t len = vsnprintf(NULL, 0, format, args2);
	if (offset + len > buf->size) {
		size_t new_size = buf->size * STRBUF_GROWTH_RATE;
		if (new_size < len + offset) new_size = len + offset;

		strbuf_resize(buf, new_size);
	}

	vsnprintf(buf->str + offset, len + 1, format, args);
	buf->offset = len + offset;

	va_end(args2);
	va_end(args);
}


void
strbuf_printf(strbuf *buf, size_t offset, char *format, ...) {
	va_list args;
	va_start(args, format);

	strbuf_vprintf_at(buf, offset, format, args);

	va_end(args);
}


void
strbuf_append(strbuf *buf, char *format, ...) {
	va_list args;
	va_start(args, format);

	strbuf_vprintf_at(buf, buf->offset, format, args);

	va_end(args);
}


char *
strbuf_copy(strbuf *buf) {
	char *str = malloc(buf->offset + 1);
	for (uint i = 0; i < buf->offset; i++) {
		str[i] = buf->str[i];
	}
	str[buf->offset] = '\0';

	return str;
}


void
strbuf_free(strbuf *buf) {
	free(buf->str);
}
