#include <stdlib.h>
#include <stdio.h>

int main() {
	int *arr = malloc(4 * sizeof(int));

	for (int i = 0; i <= 4; i++) {
		arr[i] = i;
	}

	free(arr);
}
