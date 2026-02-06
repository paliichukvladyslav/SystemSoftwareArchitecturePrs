#include <stdio.h>

#include <stdint.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
	time_t maxtime = (time_t)NULL;
	
	if (argv[1] == NULL) {
		printf("Usage: %s --32bit or %s --64bit\n", argv[0], argv[0]);
		return 1;
	}

	if (memcmp(argv[1], "--32bit", 8) == 0) {
		maxtime = (time_t)INT32_MAX;
		printf("Max time_t value is %lld\n", (long long)maxtime);
	}

	if (memcmp(argv[1], "--64bit", 8) == 0) {
		maxtime = (time_t)INT64_MAX;
		printf("Max time_t value is %lld\n", (long long)maxtime);
	}

	struct tm *tm = gmtime(&maxtime);
	if (!tm) {
		fprintf(stderr, "gmtime conversion failed, probably value out of supported range\n");
	}

	printf("End of time_t (UTC): %s", tm ? asctime(tm) : "unfortunately, null!\n");

	return 0;
}
