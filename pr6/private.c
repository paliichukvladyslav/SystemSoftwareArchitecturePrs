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

	int fd = open("mmapfile_private.bin", O_RDWR | O_CREAT, 755);
	ftruncate(fd, BUF_SIZE);

	char *buf = mmap(NULL, BUF_SIZE, 
					PROT_READ | PROT_WRITE,
				 	MAP_PRIVATE,
					fd, 0);

	printf("Parent PID: %d\n", getpid());

	pid_t pid = fork();
	
	if (pid < 0) {
		fprintf(stderr, "fork() failed!\n");
		exit(1);
	} else if (pid == 0) {
		/* child process */
		printf("Child PID: %d\n", getpid());
		puts("child pause before write");
		pause();

		for (size_t i = 0; i < BUF_SIZE; i += 4096) {
			buf[i] = 10;
		}

		puts("child pause post-write");
		pause();
	} else {
		/* parent process */
		pause();
	}

}
