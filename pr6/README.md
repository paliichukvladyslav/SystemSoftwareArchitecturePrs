# Практична робота №6

## Постановка завдання

```
Замапити файл у пам’ять у режимах shared і private, змінити його вміст і проаналізувати поведінку dirty pages.
```

## Хід роботи

Для дослідження було написано дві програми на мові C, що використовують системний виклик mmap() для відображення файлу розміром 64 МБ у віртуальний адресний простір процесу.
1. Shared mode (MAP_SHARED): Зміни, внесені одним процесом, видимі іншим і синхронізуються з файлом на диску.
2. Private mode (MAP_PRIVATE): Використовує механізм Copy-on-Write (CoW). Зміни залишаються локальними для процесу і не потрапляють у вихідний файл.

Аналіз проводився шляхом зчитування /proc/PID/smaps_rollup до та після модифікації даних у пам'яті.

## Код

private.c:

```c
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
```

shared.c:

```c
include <stdio.h>
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
```

## Результати

У режимі MAP_SHARED виділена пам'ять функціонує як єдиний спільний ресурс. Під час запису в буфер метрика Shared_Dirty у виводі smaps_rollup зростає, оскільки модифіковані сторінки стають "брудними" в кеші (page cache) і згодом синхронізуються з вихідним файлом на диску, роблячи зміни видимими для всіх процесів.
Натомість режим MAP_PRIVATE базується на механізмі Copy-on-Write (CoW). До моменту першого запису процеси спільно читають одні й ті самі сторінки пам'яті, проте спроба модифікації змушує ядро створити незалежну фізичну копію сторінки для конкретного процесу. Це призводить до різкого збільшення показника Private_Dirty - такі змінені сторінки належать виключно процесу-ініціатору, а оригінальний файл на диску залишається абсолютно незмінним.
