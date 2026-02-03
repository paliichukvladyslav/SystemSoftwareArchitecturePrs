#ifndef RBUF_H
#define RBUF_H

#include <stdbool.h>

typedef struct RingBuffer {
	char **lines;
	size_t capacity;
	size_t head;
	size_t size;
} RingBuffer;

void rbuf_init(RingBuffer *rb, int n);
void rbuf_push(RingBuffer *rb, const char *line);
void rbuf_print(RingBuffer *rb, bool reverse);
#endif
