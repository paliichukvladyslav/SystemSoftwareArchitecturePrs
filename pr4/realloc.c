#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
	size_t small = 1024;
	void *ptr = malloc(small);

	if (!ptr) {
		perror("malloc");
		return 1;
	}

	printf("Initial ptr = %p\n", ptr);

	// approx. 4 exabytes
	size_t huge = (size_t)1 << 62;

	void *tmp = realloc(ptr, huge);

	if (tmp == NULL) {
		printf("realloc failed, returned NULL\n");
		printf("original ptr is still valid: %p\n", ptr);

		((char*)ptr)[0] = 'A';
		printf("write to old memory succeeded\n");

		free(ptr);
	} else {
		printf("realloc succeeded: %p\n", tmp);
		free(tmp);
	}

	return 0;
}
