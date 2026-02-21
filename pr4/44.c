#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	void *ptr = NULL;
	size_t n = 16;

	for (int i = 0; i < 3; i++) {
		if (!ptr)
			ptr = malloc(n);

		printf("iteration %d, ptr = %p\n", i, ptr);

		memset(ptr, 0, n);

		free(ptr);
		ptr = NULL;
	}

	return 0;
}
