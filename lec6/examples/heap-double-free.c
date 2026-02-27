#include <stdlib.h>

int main() {
	int *p = malloc(16);

	free(p);
	free(p);

	return 0;
}
