#ifndef STRBUF_H_
#define STRBUF_H_

#include <stdarg.h>


#define STRBUF_INIT_SIZE 1024
#define STRBUF_GROWTH_RATE 2


typedef struct {
	char *str;
	size_t size; // excl. the '\0'
	size_t offset; // offset always points to the last written '\0'
} strbuf;


/*
 * Initialize empty string buffer.
 */
void strbuf_init(strbuf *buf);


/*
 * Resize the buffer.
 */
void strbuf_resize(strbuf *buf, size_t new_size);


/*
 * Print at a specific offset of the buffer (va_list).
 */
void strbuf_vprintf_at(strbuf *buf, size_t offset, char *format, va_list args);


/*
 * Print at a specific offset of the buffer.
 */
void strbuf_printf(strbuf *buf, size_t offset, char *format, ...);


/*
 * Append string to the end of the buffer.
 */
void strbuf_append(strbuf *buf, char *format, ...);


/*
 * Copy the string into different memory and return the only pointer to it.
 */
char *strbuf_copy(strbuf *buf); 


/*
 * Free memory used by the buffer.
 */
void strbuf_free(strbuf *buf);


#endif
