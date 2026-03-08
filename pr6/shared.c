#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <sys/mman.h>

#include <fcntl.h>

#define BUF_SIZE (64 * 1024 * 1024)

void sig_stub(int sig) {
	/* do nothing */
}

int main(void) {
	signal(SIGINT, sig_stub);
	signal(SIGUSR1, sig_stub);

	int fd = open("mmapfile_shared.bin", O_RDWR | O_CREAT, 755);
	ftruncate(fd, BUF_SIZE);

	printf("Parent PID: %d\n", getpid());

	/* shared memory */
	char *buf = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	pid_t pid = fork();
	
	if (pid < 0) {
		fprintf(stderr, "fork() failed!\n");
		exit(1);
	} else if (pid == 0) {
		/* child process */
		printf("Child PID: %d\n", getpid());
		puts("child pause before write to shared memory");
		pause();

		for (size_t i = 0; i < BUF_SIZE; i += 4096) {
			buf[i] = 10;
		}

		puts("child pause post-write");
		pause();
	} else {
		/* parent process */
		puts("parent pause before write");
		pause();

		for (size_t i = 0; i < BUF_SIZE / 2; i += 4096) {
			buf[i] = 15;
		}

		puts("parent pause after read");
		pause();
	}

}
