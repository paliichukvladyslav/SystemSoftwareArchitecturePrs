/* lottery */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

void cpu_limit_handler(int sig) {
	printf("\nCPU time limit exceeded (SIGXCPU received)\n");
	exit(0);
}

void generate_unique(int *arr, int count, int max) {
	int used[100] = {0};
	int i = 0;

	while (i < count) {
		int num = rand() % max + 1;
		if (!used[num]) {
			used[num] = 1;
			arr[i++] = num;
		}
	}
}

int main() {
	signal(SIGXCPU, cpu_limit_handler);

	srand(time(NULL));

	int a[7];
	int b[6];

	while (1) {
		generate_unique(a, 7, 49);
		generate_unique(b, 6, 36);

		printf("7 of 49: ");
		for (int i = 0; i < 7; i++) printf("%d ", a[i]);

		printf(" | 6 of 36: ");
		for (int i = 0; i < 6; i++) printf("%d ", b[i]);

		printf("\n");
	}

	return 0;
}
