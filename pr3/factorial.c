/* factorial */
#include <stdio.h>

unsigned long long fact(unsigned long long n) {
	return n * fact(n - 1);
}

int main() {
	unsigned long long n = 1000000;
	printf("calculating factorial of %llu...\n", n);

	unsigned long long result = fact(n);

	printf("result: %llu\n", result);
	return 0;
}
