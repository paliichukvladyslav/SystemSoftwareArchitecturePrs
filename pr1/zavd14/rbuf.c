#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rbuf.h"

void rbuf_init(RingBuffer *rb, int n) {
	rb->lines = calloc(n, sizeof(char *));
	rb->capacity = n;
	rb->head = 0;
	rb->size = 0;
}

void rbuf_push(RingBuffer *rb, const char *line) {
	if (rb->lines[rb->head]) {
		free(rb->lines[rb->head]);
	}

	rb->lines[rb->head] = strdup(line);
	rb->head = (rb->head + 1) % rb->capacity;

	if (rb->size < rb->capacity) rb->size++;
}

void rbuf_print(RingBuffer *rb) {
	for (int i = 0; i < rb->size; i++) {
		int idx = (rb->head - 1 - i + rb->capacity) % rb->capacity;
		if (rb->lines[idx]) printf("%s", rb->lines[idx]);
	}
}
