#include <stdio.h>

#include "rbuf.h"

#define BUF_SIZE (128 * 1024)

int main(int argc, char *argv[]) {
	int n = 10;

	RingBuffer rb;
	rbuf_init(&rb, n);

	FILE *input = fopen("/etc/ca-certificates.conf", "r");
	
	char buffer[BUF_SIZE];
	while (fgets(buffer, sizeof(buffer), input)) {
		rbuf_push(&rb, buffer);
	}

	rbuf_print(&rb);
	
	return 0;
}
