#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
	int xa = 50000;
	int xb = 50000;

	int num = xa * xb;

	printf("xa = %d, xb = %d\n", xa, xb);
	printf("num (signed int) = %d\n", num);
	printf("num (as size_t) = %zu\n", (size_t)num);

	void *p = malloc(num);

	if (p == NULL) {
		perror("malloc failed");
	} else {
		printf("malloc succeeded: %p\n", p);
		free(p);
	}

	return 0;
}
