#include <stdio.h>
#include <stdlib.h>

int main() {
	void *a = realloc(NULL, 16);
	printf("realloc(NULL,16) = %p\n", a);

	void *b = malloc(16);
	void *r = realloc(b, 0);
	printf("realloc(b,0) = %p\n", r);

	free(a);
	return 0;
}
