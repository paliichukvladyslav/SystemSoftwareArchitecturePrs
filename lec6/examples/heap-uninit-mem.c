#include <stdio.h>
#include <stdlib.h>

int main() {
	int *arr = malloc(5 * sizeof(int));

	arr[0] = 10;
	arr[1] = 20;

	int sum = arr[0] + arr[1] + arr[2];

	printf("sum = %d\n", sum);

	free(arr);
	return 0;
}
