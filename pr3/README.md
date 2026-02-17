# Практична робота №3

**Тема:** Дослідження обмежень ресурсів у середовищі Docker

## Середовище виконання

Зважаючи на ряд певних технічних та особистих причин, практичну роботу буде виконано у хмарному середовищі Google Cloud Compute Engine. Щоб забезпечити відтворюване середовище для лабораторної работи, воно буде задокументоване нижче.

Створімо нову віртуальну машину:
```bash
gcloud compute instances create lab3 \
	--zone=europe-west1-b \
	--machine-type=c4d-standard-4 \
	--image-family=ubuntu-2204-lts \
	--image-project=ubuntu-os-cloud \
	--boot-disk-size=30G \
	--provisioning-model=SPOT \
	--instance-termination-action=DELETE
```

Щоб отримати доступ до системи, виконаємо:
```bash
gcloud compute ssh lab3 --zone=europe-west1-b
```

Увійшовши у систему, розгорнемо середовище виконання лабораторної работи:
```bash
sudo apt update && sudo apt upgrade -y
sudo apt install -y docker.io
sudo systemctl enable docker --now
sudo usermod -aG docker $USER
```

Після цього необхідно перезайти:
```bash
exit
gcloud compute ssh lab3 --zone=europe-west1-b
```

Пересвідчуємося, що docker працює:
```bash
docker run hello-world
```
Отримуємо вивід:
```bash
Unable to find image 'hello-world:latest' locally
latest: Pulling from library/hello-world
17eec7bbc9d7: Pull complete
Digest: sha256:ef54e839ef541993b4e87f25e752f7cf4238fa55f017957c2eb44077083d7a6a
Status: Downloaded newer image for hello-world:latest

Hello from Docker!
This message shows that your installation appears to be working correctly.

To generate this message, Docker took the following steps:
 1. The Docker client contacted the Docker daemon.
 2. The Docker daemon pulled the "hello-world" image from the Docker Hub.
    (amd64)
 3. The Docker daemon created a new container from that image which runs the
    executable that produces the output you are currently reading.
 4. The Docker daemon streamed that output to the Docker client, which sent it
    to your terminal.

To try something more ambitious, you can run an Ubuntu container with:
 $ docker run -it ubuntu bash

Share images, automate workflows, and more with a free Docker ID:
 https://hub.docker.com/

For more examples and ideas, visit:
 https://docs.docker.com/get-started/
```

Дозволяємо ядру усі події моніторингу (необхідно для perf):
```bash
sudo sysctl -w kernel.perf_event_paranoid=-1
```

Після цього вже можна створити основний контейнер для всіх наступних завдань:
```bash
docker run --privileged -it fedora:43 bash
```

Лишилося тільки встановити у контейнер необхідні для роботи пакети:

```bash
dnf -y update
dnf install -y gcc make util-linux procps-ng vim perf
```

## Завдання 3.1

```bash
[root@02244ace0c72 /]# ulimit -n
1048576
[root@02244ace0c72 /]# ulimit -aS | grep "open files"
open files                          (-n) 1048576
[root@02244ace0c72 /]# ulimit -aH | grep "open files"
open files                          (-n) 1048576
[root@02244ace0c72 /]# ulimit -n 3000
[root@02244ace0c72 /]# ulimit -aS | grep "open files"
open files                          (-n) 3000
[root@02244ace0c72 /]# ulimit -aH | grep "open files"
open files                          (-n) 3000
[root@02244ace0c72 /]# ulimit -n 3001
bash: ulimit: open files: cannot modify limit: Operation not permitted
[root@02244ace0c72 /]# ulimit -n 2000
[root@02244ace0c72 /]# ulimit -n
2000
[root@02244ace0c72 /]# ulimit -aS | grep "open files"
open files                          (-n) 2000
[root@02244ace0c72 /]# ulimit -aH | grep "open files"
open files                          (-n) 2000
[root@02244ace0c72 /]# ulimit -n 3000
bash: ulimit: open files: cannot modify limit: Operation not permitted
[root@02244ace0c72 /]# 
```

У контейнері початково soft і hard ліміти на кількість відкритих файлів збігалися (1048576), що є типовим для Docker-середовища. Оскільки експеримент виконувався від імені root, при встановленні нового значення ulimit -n 3000 було змінено як soft, так і hard limit. Після зменшення hard limit подальше збільшення значення (наприклад до 3001) стало неможливим, що підтверджує правило: процес не може перевищити встановлений hard limit.

## Завдання 3.2

### Код

```c
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
```

### Компіляція та запуск

```bash
[root@0ddbd513100c /]# make ofl
cc     ofl.c   -o ofl
[root@0ddbd513100c /]# ulimit -n 1584
[root@0ddbd513100c /]# ulimit -n
1584
[root@0ddbd513100c /]# perf stat -e task-clock,context-switches,page-faults,syscalls:sys_enter_openat,syscalls:sys_exit_openat ./ofl
open failed: Too many open files
Reached limit after 1581 open files

 Performance counter stats for './ofl':

           1870699      task-clock                       #    0.001 CPUs utilized             
                 1      context-switches                 #  534.560 /sec                      
                63      page-faults                      #   33.677 K/sec                     
              1584      syscalls:sys_enter_openat        #  846.742 K/sec                     
              1584      syscalls:sys_exit_openat         #  846.742 K/sec                     

       2.002216636 seconds time elapsed

       0.000000000 seconds user
       0.002158000 seconds sys
```

За допомогою perf ми можемо бачити 1584 спроб використати системний виклик open.

## Завдання 3.3

### Код

```c
/* dice */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

FILE *f;

void file_limit_handler(int sig) {
	puts("file size limit reached (SIGXFSZ received)");
	if (f) fclose(f);
	exit(0);
}

int main() {
	signal(SIGXFSZ, file_limit_handler);

	f = fopen("dice.txt", "w");
	if (!f) {
		fprintf(stderr, "cannot open file for writing\n");
		return 1;
	}

	srand(time(NULL));

	long count = 0;

	while (1) {
		int roll = rand() % 6 + 1;
		if (fprintf(f, "%d\n", roll) < 0) {
			fprintf(stderr, "write error\n");
			break;
		}
		count++;
	}

	fclose(f);
	return 0;
}
```

### Компіляція та запуск

```bash
[root@0ddbd513100c /]# make dice
cc     dice.c   -o dice
[root@0ddbd513100c /]# ulimit -f 10
[root@0ddbd513100c /]# ./dice
file size limit reached (SIGXFSZ received)
[root@0ddbd513100c /]# tail dice.txt 
4
5
2
1
5
3
2
4
3
2
[root@0ddbd513100c /]# 
```

Під час виконання програма безперервно генерує випадкові значення від 1 до 6 та записує їх у файл. Після досягнення встановленого ліміту розміру файлу операційна система генерує сигнал SIGXFSZ, який було оброблено в програмі за допомогою обробника сигналів. У момент отримання сигналу програма коректно завершує роботу: виводить повідомлення про перевищення ліміту, закриває файл та завершує виконання без аварійного завершення.

## Завдання 3.4

### Код

```c
/* lottery */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

void cpu_limit_handler(int sig) {
        printf("CPU time limit exceeded (SIGXCPU received)\n");
        exit(0);
}

void generate_unique(int *arr, int count, int max) {
        int used[100] = {0};
        int i = 0;

        while (i < count) {
                int num = rand() % max + 1;
                if (!used[num]) {
                        used[num] = 1;
                        arr[i++] = num;
                }
        }
}

int main() {
        signal(SIGXCPU, cpu_limit_handler);

        srand(time(NULL));

        int a[7];
        int b[6];

        while (1) {
                generate_unique(a, 7, 49);
                generate_unique(b, 6, 36);

                printf("7 of 49: ");
                for (int i = 0; i < 7; i++) printf("%d ", a[i]);

                printf(" | 6 of 36: ");
                for (int i = 0; i < 6; i++) printf("%d ", b[i]);

                printf("\n");
        }

        return 0;
}
```

### Компіляція та запуск

```bash
[root@0ddbd513100c /]# make lott 
cc     lott.c   -o lott
[root@0ddbd513100c /]# ulimit -St 1
[root@0ddbd513100c /]# ulimit -Ht 3
[root@0ddbd513100c /]# ./lott
7 of 49: 39 40 27 19 47 48 1  | 6 of 36: 29 1 20 15 4 33 
7 of 49: 21 25 43 4 3 45 39  | 6 of 36: 14 28 19 27 35 2 
7 of 49: 3 49 39 42 17 16 37  | 6 of 36: 11 24 26 3 25 17 
7 of 49: 2 16 25 26 9 28 4  | 6 of 36: 17 7 6 8 25 32 
7 of 49: 28 42 40 27 31 37 22  | 6 of 36: 3 35 12 13 2 1 
7 of 49: 30 13 45 34 14 16 9  | 6 of 36: 15 23 6 27 14 22 
7 of 49: 45 17 49 10 44 41 5  | 6 of 36: 10 33 14 19 36 28
[...]
7 of 49: 39 40 12 36 20 30 6  | 6 of 36: 30 22 19 8 28 2 
7 of 49:
CPU time limit exceeded (SIGXCPU received)
```

## Завдання 3.5

### Код

```c
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
```

### Компіляція та запуск

```bash
[root@c4b5da655477 /]# make copy
cc     copy.c   -o copy
[root@c4b5da655477 /]# ulimit -f 1
[root@c4b5da655477 /]# ulimit -f
1
[root@c4b5da655477 /]# ./copy /etc/pki/tls/openssl.cnf out
File size limit exceeded   (core dumped) ./copy /etc/pki/tls/openssl.cnf out
```
