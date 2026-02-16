/* dice */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

FILE *f;

void file_limit_handler(int sig) {
	puts("file size limit reached (SIGXFSZ received)");
	if (f) fclose(f);
	exit(0);
}

int main() {
	signal(SIGXFSZ, file_limit_handler);

	f = fopen("dice.txt", "w");
	if (!f) {
		fprintf(stderr, "cannot open file for writing\n");
		return 1;
	}

	srand(time(NULL));

	long count = 0;

	while (1) {
		int roll = rand() % 6 + 1;
		if (fprintf(f, "%d\n", roll) < 0) {
			fprintf(stderr, "write error\n");
			break;
		}
		count++;
	}

	fclose(f);
	return 0;
}
