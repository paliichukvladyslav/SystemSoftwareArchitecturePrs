# Практична робота №2
Тема: Сегменти виконуваного файлу та організація пам'яті процесу

## Завдання 1

Визначення моменту переповнення time_t

### Код програми

```c
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
```

### Компіляція та запуск

```bash
make time
./time --32bit
./time --64bit
```

### Результат виконання

```bash
$ ./time --32bit
Max time_t value is 2147483647
End of time_t (UTC): Tue Jan 19 03:14:07 2038
$ ./time --64bit
Max time_t value is 9223372036854775807
gmtime conversion failed, probably value out of supported range
End of time_t (UTC): unfortunately, null!
```

### Висновок

Максимальне значення time_t для 32-бітних систем відповідає даті 19 січня 2038 року. Однак, 64-бітне значення time_t є занадто великим і не може бути конвертоване в дату gmtime(3).

## Завдання 2.2

### Підзавдання 1
Напишемо базовий hello world:

```c
include <stdio.h>

int main(void) {
	puts("Hello World!");
	return 0;
}
```

Виконаємо size:

```bash
$ size helloworld
   text    data     bss     dec     hex filename
   1642     624       8    2274     8e2 helloworld
```

### Підзавдання 2

Додамо глобальний масив із 1000 int:

```c
include <stdio.h>

int arr[1000];

int main(void) {
	puts("Hello World!");
	return 0;
}
```

Виконаємо size:

```bash
$ size helloworld
   text    data     bss     dec     hex filename
   1642     624    4008    6274    1882 helloworld
```

Бачимо, що до секції .bss додалося 4000 байт, оскільки ми додали неініціалізований масив на 1000 елементів типу int (4 байти).

### Підзавдання 3

Додамо початкові значення:

```c
include <stdio.h>

int arr[1000] = { 5, 5, 5, 5 };

int main(void) {
	puts("Hello World!");
	return 0;
}
```

Повторимо вимірювання:

```bash
$ size helloworld
   text    data     bss     dec     hex filename
   1642    4624       8    6274    1882 helloworld
```

Бачимо, що масив дійсно переїхав з `.bss` у `.data`.

### Підзавдання 4

```c
#include <stdio.h>

int arr[1000] = { 5, 5, 5, 5 };

int main(void) {
        puts("Hello World!");
void func(void) {
        int arr2[3582] = { 1, 2 ,3 };
}
        return 0;
}
```

Повторимо вимірювання:

```bash
$ size helloworld
   text    data     bss     dec     hex filename
   1650    4624       8    6282    188a helloworld
```

Бачимо, що локальні масиви, чи то ініціалізовані чи ні, не залишаються у виконуваному файлі, натомість створюючись у рантаймі.

## Завдання 2.3

### Код програми

```c
#include <stdio.h>

int bss;
int data = 10;

int func(void) {
        return 10;
}

int main(void) {
        int i;
        printf("The stack top is near %p\n", &i);
        printf("bss: %p\n", &bss);
        printf("data: %p\n", &data);
        printf("text: %p\n", func);

        int arr[400];
        for (int i = 0; i < 400; i++) {
                arr[i] = func();
        }
        int j;

        printf("(2) The stack top is near %p\n", &j);
        return 0;
}
```

### Компіляція та запуск

```bash
make st1
./st1
```

### Результат виконання

```bash
The stack top is near 0x7fcc60d9b8
bss: 0x5580c70040
data: 0x5580c70038
text: 0x5580c507a8
(2) The stack top is near 0x7fcc60d374
```

## Завдання 2.4

### gstack

Спочатку отримаємо PID нашого процесу `bash`:

```bash
$ pidof bash
2016
```

Тепер застосуємо утиліту `gstack`:

```bash
$ gstack 2016
warning: 40     ./nptl/cancellation.c: No such file or directory
Thread 1 (Thread 0x7f84cc5020 (LWP 2016) "bash"):
#0  __internal_syscall_cancel (a1=-1, a2=549182653280, a3=10, a4=0, a5=a5@entry=0, a6=a6@entry=0, nr=nr@entry=260) at ./nptl/cancellation.c:40
#1  0x0000007f84b1261c in __syscall_cancel (a1=<optimized out>, a2=<optimized out>, a3=<optimized out>, a4=<optimized out>, a5=a5@entry=0, a6=a6@entry=0, nr=nr@entry=260) at ./nptl/cancellation.c:75
#2  0x0000007f84b6a9a4 in __GI___wait4 (pid=<optimized out>, stat_loc=<optimized out>, options=<optimized out>, usage=<optimized out>) at ../sysdeps/unix/sysv/linux/wait4.c:30
#3  0x0000005587345984 in ?? ()
#4  0x00000055873471b8 in wait_for ()
#5  0x000000558733054c in execute_command_internal ()
#6  0x0000005587330ce4 in execute_command ()
#7  0x00000055873161d4 in reader_loop ()
#8  0x0000005587314600 in main ()
```

### gdb

Скопіюємо та вставимо код завдання у файл `24.c` (див. файли репозиторію)
Скомпілюємо, запустимо та отримаємо PID програми:

```bash
$ make 24
cc     24.c   -o 24
$ ./24 &
[2] 2325
$ In function                 main; &localvar = 0x7ff4e380ac
In function                  foo; &localvar = 0x7ff4e3807c
In function                  bar; &localvar = 0x7ff4e3805c
In function    bar_is_now_closed; &localvar = 0x7ff4e3803c

 Now blocking on pause()...

$ pidof 24
2325
```

Тепер під'єднаємося за допомогою `gdb`:

```bash
dietpi@DietPi:~$ gdb attach 2325
GNU gdb (Debian 16.3-1) 16.3
Copyright (C) 2024 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "aarch64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
attach: No such file or directory.
Attaching to process 2325
Reading symbols from /home/dietpi/remclones/SystemSoftwareArchitecturePrs/pr2/24...
(No debugging symbols found in /home/dietpi/remclones/SystemSoftwareArchitecturePrs/pr2/24)
Reading symbols from /lib/aarch64-linux-gnu/libc.so.6...
Reading symbols from /usr/lib/debug/.build-id/42/3c491945894188fffd2c28575f5fd7b4f3b064.debug...
Reading symbols from /lib/ld-linux-aarch64.so.1...
Reading symbols from /usr/lib/debug/.build-id/3e/8ab1e1101f7e5e8b9be1f8c4027f083f996e1a.debug...
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/aarch64-linux-gnu/libthread_db.so.1".
__internal_syscall_cancel (a1=a1@entry=0, a2=a2@entry=0, a3=a3@entry=0, a4=a4@entry=0, a5=a5@entry=0, a6=a6@entry=0,
    nr=nr@entry=73) at ./nptl/cancellation.c:40

warning: 40     ./nptl/cancellation.c: No such file or directory
```

Виконаємо команду `bt`:

```bash
(gdb) bt
#0  __internal_syscall_cancel (a1=a1@entry=0, a2=a2@entry=0, a3=a3@entry=0, a4=a4@entry=0, a5=a5@entry=0,
    a6=a6@entry=0, nr=nr@entry=73) at ./nptl/cancellation.c:40
#1  0x0000007f8034261c in __syscall_cancel (a1=a1@entry=0, a2=a2@entry=0, a3=a3@entry=0, a4=a4@entry=0, a5=a5@entry=0,
    a6=a6@entry=0, nr=nr@entry=73) at ./nptl/cancellation.c:75
#2  0x0000007f8038912c in __libc_pause () at ../sysdeps/unix/sysv/linux/pause.c:31
#3  0x00000055638f08a4 in bar_is_now_closed ()
#4  0x00000055638f08e0 in bar ()
#5  0x00000055638f091c in foo ()
#6  0x00000055638f0960 in main ()
(gdb)
```

Завдання виконано

## Завдання 2.5

Завдання:

```
Відомо, що при виклику процедур і поверненні з них процесор
використовує стек.Чи можна в такій схемі обійтися без лічильника команд
(IP), використовуючи замість нього вершину стека? Обґрунтуйте свою
відповідь та наведіть приклади.
```

Теоретично можна побудувати архітектуру без окремого регістра IP/PC, використовуючи стек або пам'ять для зберігання адрес переходів, оскільки це фізично можливо. Однак повністю обійтися без механізму, який виконує функції лічильника команд, неможливо. Стек може зберігати адреси повернення та використовуватися для передачі керування (наприклад, під час ret), але він не здатний забезпечити послідовне виконання інструкцій, цикли та умовні переходи. Тому в будь-якій реальній системі існує функціональний аналог IP, навіть якщо він не представлений як явний регістр.
Існують стекові архітектури (наприклад, Forth-системи, JVM, Burroughs B5000), у яких стек активно використовується для передачі керування та обчислень. Однак у всіх цих системах присутній механізм лічильника команд (program counter), явний або прихований. Повністю обійтися без нього неможливо, оскільки саме він забезпечує послідовне виконання інструкцій та керування переходами. Отже, стек може доповнювати механізм керування виконанням, але не може повністю його замінити.

## Завдання за варіантом 14

```
14. Реалізуйте програму для аналізу вмісту /proc/self/maps.
```

Завдання виконано у окремому підкаталозі variant14 з [окремим README.md](./variant14/README.md)
