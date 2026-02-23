# Завдання за варіантом 14

## Постановка завдання
```
Дослідити, як fork() впливає на використання пам’яті після великої алокації. Показати ефект Copy-On-Write.
```

## Виконання завдання

Було досліджено, як системний виклик fork() впливає на використання пам'яті процесом після виконання великої динамічної алокації.
Було написано програму мовою C, у якій виділяється великий буфер пам'яті за допомогою malloc та ініціалізується (щоб сторінки були реально відображені в пам'яті). Після цього викликається fork(), у результаті чого створюється дочірній процес. Обидва процеси (батьківський і дочірній) зупиняються за допомогою pause(), щоб можна було проаналізувати їх використання пам'яті.
Для дослідження використовувався файл /proc/<pid>/smaps_rollup, з якого аналізувалися поля Shared_Dirty та Private_Dirty. Ці показники дозволяють визначити, яка частина пам’яті є спільною між процесами, а яка - приватною.

### Код

```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define BUF_SIZE (64 * 1024 * 1024)

void sig_stub(int sig) {
	/* do nothing */
}

int main(void) {
	signal(SIGINT, sig_stub);
	signal(SIGUSR1, sig_stub);

	char *buf = malloc(BUF_SIZE);
	memset(buf, 0, BUF_SIZE); /* explicit memset instead of calloc */

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
```

### Компіляція та запуск

```bash
dietpi@DietPi:var14$ make cow
cc     cow.c   -o cow
dietpi@DietPi:var14$ ./cow 
Parent PID: 2601
Child PID: 2602
child pause before write
child pause post-write
```

Паралельно з цим, аналізуємо пам'ять процесів. Бачимо, що до запису майже всі сторінки знаходились у стані Shared_Dirty, а значення Private_Dirty було дуже малим. Після запису дочірнім процесом у пам'ять кожної сторінки, в smaps_rollup спостерігалося різке зменшення Shared_Dirty та значне збільшення Private_Dirty майже до повного розміру буфера для обох процесів:

```bash
dietpi@DietPi:~$ cat /proc/2601/smaps_rollup 
555d780000-7fe4969000 ---p 00000000 00:00 0                              [rollup]
Rss:               66932 kB
Pss:               32880 kB
Pss_Dirty:         32828 kB
Pss_Anon:          32828 kB
Pss_File:             52 kB
Pss_Shmem:             0 kB
Shared_Clean:       1312 kB
Shared_Dirty:      65584 kB
Private_Clean:         0 kB
Private_Dirty:        36 kB
Referenced:        66932 kB
Anonymous:         65620 kB
KSM:                   0 kB
LazyFree:              0 kB
AnonHugePages:         0 kB
ShmemPmdMapped:        0 kB
FilePmdMapped:         0 kB
Shared_Hugetlb:        0 kB
Private_Hugetlb:       0 kB
Swap:                  0 kB
SwapPss:               0 kB
Locked:                0 kB
dietpi@DietPi:~$ cat /proc/2602/smaps_rollup 
555d780000-7fe4969000 ---p 00000000 00:00 0                              [rollup]
Rss:               66836 kB
Pss:               32876 kB
Pss_Dirty:         32828 kB
Pss_Anon:          32828 kB
Pss_File:             48 kB
Pss_Shmem:             0 kB
Shared_Clean:       1216 kB
Shared_Dirty:      65584 kB
Private_Clean:         0 kB
Private_Dirty:        36 kB
Referenced:         1276 kB
Anonymous:         65620 kB
KSM:                   0 kB
LazyFree:              0 kB
AnonHugePages:         0 kB
ShmemPmdMapped:        0 kB
FilePmdMapped:         0 kB
Shared_Hugetlb:        0 kB
Private_Hugetlb:       0 kB
Swap:                  0 kB
SwapPss:               0 kB
Locked:                0 kB
dietpi@DietPi:~$ kill -USR1 2602
dietpi@DietPi:~$ cat /proc/2601/smaps_rollup 
555d780000-7fe4969000 ---p 00000000 00:00 0                              [rollup]
Rss:               66932 kB
Pss:               65650 kB
Pss_Dirty:         65598 kB
Pss_Anon:          65598 kB
Pss_File:             52 kB
Pss_Shmem:             0 kB
Shared_Clean:       1312 kB
Shared_Dirty:         44 kB
Private_Clean:         0 kB
Private_Dirty:     65576 kB
Referenced:        66932 kB
Anonymous:         65620 kB
KSM:                   0 kB
LazyFree:              0 kB
AnonHugePages:         0 kB
ShmemPmdMapped:        0 kB
FilePmdMapped:         0 kB
Shared_Hugetlb:        0 kB
Private_Hugetlb:       0 kB
Swap:                  0 kB
SwapPss:               0 kB
Locked:                0 kB
dietpi@DietPi:~$ cat /proc/2602/smaps_rollup 
555d780000-7fe4969000 ---p 00000000 00:00 0                              [rollup]
Rss:               66840 kB
Pss:               65646 kB
Pss_Dirty:         65598 kB
Pss_Anon:          65598 kB
Pss_File:             48 kB
Pss_Shmem:             0 kB
Shared_Clean:       1220 kB
Shared_Dirty:         44 kB
Private_Clean:         0 kB
Private_Dirty:     65576 kB
Referenced:        66820 kB
Anonymous:         65620 kB
KSM:                   0 kB
LazyFree:              0 kB
AnonHugePages:         0 kB
ShmemPmdMapped:        0 kB
FilePmdMapped:         0 kB
Shared_Hugetlb:        0 kB
Private_Hugetlb:       0 kB
Swap:                  0 kB
SwapPss:               0 kB
Locked:                0 kB
dietpi@DietPi:~$ 
```
