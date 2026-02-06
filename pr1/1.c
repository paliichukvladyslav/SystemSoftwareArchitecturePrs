#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <fenv.h>

int main(void) {
	double z;
	puts("Enter Z-score: ");


	if (scanf("%lf", &z) != 1) {
		fprintf(stderr, "Invalid input!\n");
		return 1;
	}

	feclearexcept(FE_ALL_EXCEPT);
	float sigma = 1.645;
	errno = 0;

	printf("The integral of a Normal (0,1) distribution " "between -Z and Z i’s: %g\n", erf(z / (sigma * sqrt(2))));
	printf("errno (90%%): %d\n", errno);

	feclearexcept(FE_ALL_EXCEPT);
	errno = 0;
	printf("The integral of a Normal (0,1) distribution " "between -Z and Z i’s: %g\n", erf(z * sqrt(1/2)));
	printf("errno (95%%): %d\n", errno);

	feclearexcept(FE_ALL_EXCEPT);
	printf("The integral of a Normal (0,1) distribution " "between -Z and Z i’s: %g\n", erf(z / sqrt(2)));
	printf("errno (99%%): %d\n", errno);

	return 0;
}
