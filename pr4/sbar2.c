#include <stdio.h>
#include <stdlib.h>

struct sbar {
	float x, y;
};

int main(void) {
	struct sbar *ptr, *newptr;

	ptr = calloc(1000, sizeof(struct sbar));

	newptr = reallocarray(ptr, 500, sizeof(struct sbar));
	if (newptr == NULL) {
		free(ptr);
		return 1;
	}
	ptr = newptr;
}
