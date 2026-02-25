#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE (8L * 1024L * 1024L * 1024L)

int main(void) {
	printf("Attempting to allocate %zuG...\n", SIZE / 1024 / 1024 / 1024);

	char *p = malloc(SIZE);
	if (!p) {
		fprintf(stderr, "malloc failed\n");
		return 1;
	}

	puts("malloc succeeded, writing to memory...");

	for (size_t i = 0; i < SIZE; i += 4096) {
		p[i] = 99;

		if ( (i / 4096) % 64 == 0 ) {
			usleep(5000);
		}
	}

	puts("done");
	free(p);
	return 0;
}
