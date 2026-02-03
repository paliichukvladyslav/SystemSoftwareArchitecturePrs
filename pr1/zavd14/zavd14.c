#include <stdio.h>
#include <unistd.h>

#include <stdint.h>
#include <errno.h>
#include <stdlib.h>

#include "rbuf.h"

#define BUF_SIZE (128 * 1024)

int main(int argc, char *argv[]) {
	size_t n;
	bool reverse = false;

	unsigned long n_tmp;
	char *end_tmp;

	errno = 0;
	int opt;
	while ( (opt = getopt(argc, argv, "n:r")) != -1 ) {
		switch (opt) {
		case 'r':
			reverse = true;
			break;
		case 'n':
			if (optarg[0] == '-') {
				fprintf(stderr, "size must be non-negative\n");
				exit(1);
			}
			n_tmp = strtoul(optarg, &end_tmp, 10);
		}
	}

	if (errno != 0 || *end_tmp != '\0' || n_tmp > SIZE_MAX) {
		fprintf(stderr, "invalid size provided: %s\n", optarg);
		exit(1);
	}

	n = (size_t)n_tmp;

	RingBuffer rb;
	rbuf_init(&rb, n);

	FILE *input = fopen("/etc/ca-certificates.conf", "r");
	
	char buffer[BUF_SIZE];
	while (fgets(buffer, sizeof(buffer), input)) {
		rbuf_push(&rb, buffer);
	}

	rbuf_print(&rb, reverse);
	
	return 0;
}
