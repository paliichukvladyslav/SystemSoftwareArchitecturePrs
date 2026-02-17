/* copy */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Program needs at least two arguments\n");
		return 1;
	}

	char *src = argv[1];
	char *dst = argv[2];

	FILE *in = fopen(src, "rb");
	if (!in) {
		fprintf(stderr, "Cannot open file %s for reading\n", src);
		return 1;
	}

	FILE *out = fopen(dst, "wb");
	if (!out) {
		fprintf(stderr, "Cannot open file %s for writing\n", dst);
		fclose(in);
		return 1;
	}

	char buffer[BUF_SIZE];

	size_t nread;
	while ((nread = fread(buffer, 1, BUF_SIZE, in)) > 0) {

		size_t nwritten = fwrite(buffer, 1, nread, out);

		if (nwritten < nread) {
			if (ferror(out)) {
				if (errno == EFBIG) {
					fprintf(stderr, "File size limit exceeded while writing\n");
				} else {
					fprintf(stderr, "Write error: %s\n", strerror(errno));
				}
			}
			fclose(in);
			fclose(out);
			return 1;
		}
	}

	if (ferror(in)) {
		fprintf(stderr, "Read error: %s\n", strerror(errno));
	}

	fclose(in);
	fclose(out);
	return 0;
}
