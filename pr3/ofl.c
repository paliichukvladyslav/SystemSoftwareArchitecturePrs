/* open file limit */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MAX_FILES 100000

int main() {
	int fds[MAX_FILES];
	int count = 0;

	while (count < MAX_FILES) {
		int fd = open("/dev/null", O_RDONLY);
		if (fd == -1) {
			perror("open failed");
			printf("Reached limit after %d open files\n", count);
			break;
		}
		fds[count++] = fd;
	}

	sleep(2);

	for (int i = 0; i < count; i++) {
		close(fds[i]);
	}

	return 0;
}
