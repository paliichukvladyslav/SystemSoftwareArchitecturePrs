# Практична робота №4

**Тема**: Динамічне виділення пам'яті

## Завдання 4.1

malloc приймає розмір типу size_t, а це беззнакове 64-бітне число, тому максимальне значення - 2^64-1 (мінус один з'являється тому, що нуль займає одне з можливих значень), але це лише "теоретична стеля" для числа, яке можна передати в malloc, а не реальна кількість пам'яті.
Реальною межею є віртуальний адресний простір процесу: у звичайному x86_64 використовують 48 бітні адреси (це 256 ТБ загалом), і ця область ділиться навпіл між програмами та ядром (приблизно по 128 ТБ), причому програма може використовувати тільки свою половину, і з неї ще віднімаються стек, області mmap, бібліотеки тощо.Саме це розділення робить ядро щоб захистити систему. У новіших режимах (LA57) адресних бітів більше, але принцип той самий.
Також важливо, що malloc насправді не "створює пам'ять із повітря", а просить її в ядра через механізми brk або mmap, і адреси мають бути "канонічними" (старші біти повторюють знак молодшого бітового розряду адреси). Тому число 2^64-1 - це лише максимально можливий розмір аргументу, а реальна межа виділення визначається доступним користувацьким віртуальним адресним простором і обмеженнями ядра.

## Завдання 4.2

### Код

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
	int xa = 50000;
	int xb = 50000;

	int num = xa * xb;

	printf("xa = %d, xb = %d\n", xa, xb);
	printf("num (signed int) = %d\n", num);
	printf("num (as size_t) = %zu\n", (size_t)num);

	void *p = malloc(num);

	if (p == NULL) {
		perror("malloc failed");
	} else {
		printf("malloc succeeded: %p\n", p);
		free(p);
	}

	return 0;
}
```

### Компіляція та запуск

```bash
dietpi@DietPi:pr4$ make overflow
cc     overflow.c   -o overflow
dietpi@DietPi:pr4$ ./overflow 
xa = 50000, xb = 50000
num (signed int) = -1794967296
num (as size_t) = 18446744071914584320
malloc failed: Cannot allocate memory
```

Якщо передати від'ємний аргумент у malloc, він не буде відхилений одразу, а буде неявно перетворений до типу size_t, що призведе до дуже великого додатного значення.
Якщо ж розмір обчислюється як добуток двох знакових цілих і виникає переповнення, результат може стати від'ємним (невизначена поведінка, але на практиці - wrap-around). Після приведення до size_t це значення знову стає великим додатним числом, і malloc намагається виділити надзвичайно великий блок пам'яті, що зазвичай завершується помилкою та поверненням NULL.

## Завдання 4.3

### Код

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
	void *p = malloc(0);

	if (p == NULL) {
		printf("malloc(0) returned NULL\n");
	} else {
		printf("malloc(0) returned non-NULL pointer: %p\n", p);
	}

	free(p);
	printf("free(p) completed\n");

	return 0;
}
```

### Компіляція та запуск

```bash
dietpi@DietPi:pr4$ make zero
cc     zero.c   -o zero
dietpi@DietPi:pr4$ ./zero 
malloc(0) returned non-NULL pointer: 0x557f7df2a0
free(p) completed
dietpi@DietPi:pr4$ ltrace ./zero 
__libc_start_main(["./zero"] <unfinished ...>
malloc(0)                                                  = 0x55c580e2a0
printf("malloc(0) returned non-NULL poin"...malloc(0) returned non-NULL pointer: 0x55c580e2a0
)              = 50
free(0x55c580e2a0)                                         = <void>
puts("free(p) completed"free(p) completed
)                                  = 18
__cxa_finalize(0x5592c50048)                               = <void>
+++ exited (status 0) +++
```

Відповідно до стандарту C, виклик malloc(0) може повернути або NULL, або ненульовий вказівник, який дозволено передати у free().
У glibc на Linux зазвичай повертається ненульовий вказівник на мінімальний службовий блок пам'яті (chunk), який не призначений для використання, але може бути коректно звільнений через free().
При трасуванні через ltrace видно, що виклики malloc(0) і free() відбуваються коректно, і free() не викликає помилок.
Розіменування такого вказівника є невизначеною поведінкою.

## Завдання 4.4

###

###

У наведеному у завданні коді є помилка: після виклику free(ptr) змінна ptr не обнуляється і залишається містити стару адресу звільненого блоку пам'яті. На наступній ітерації умова if (!ptr) не виконується, тому malloc не викликається, і програма продовжує використовувати вже звільнену пам'ять (use-after-free). Це призводить до невизначеної поведінки.

Напишімо програму, що демонструє цей випадок:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	void *ptr = NULL;
	size_t n = 16;

	for (int i = 0; i < 3; i++) {
		if (!ptr)
			ptr = malloc(n);

		printf("iteration %d, ptr = %p\n", i, ptr);

		memset(ptr, 0, n);

		free(ptr);
	}

	return 0;
}
```

Виконаймо компіляцію та запуск:

```bash
dietpi@DietPi:pr4$ make 44
cc     44.c   -o 44
dietpi@DietPi:pr4$ ./44
iteration 0, ptr = 0x55a593e2a0
iteration 1, ptr = 0x55a593e2a0
iteration 2, ptr = 0x55a593e2a0
dietpi@DietPi:pr4$ valgrind ./44
==16129== Memcheck, a memory error detector
==16129== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==16129== Using Valgrind-3.24.0 and LibVEX; rerun with -h for copyright info
==16129== Command: ./44
==16129==
iteration 0, ptr = 0x4a7f040
iteration 1, ptr = 0x4a7f040
==16129== Invalid write of size 8
==16129==    at 0x4892B0C: memset (vg_replace_strmem.c:1390)
==16129==    by 0x1088BF: main (in /home/dietpi/remclones/SystemSoftwareArchitecturePrs/pr4/44)
==16129==  Address 0x4a7f040 is 0 bytes inside a block of size 16 free'd
==16129==    at 0x48884F8: free (vg_replace_malloc.c:989)
==16129==    by 0x1088C7: main (in /home/dietpi/remclones/SystemSoftwareArchitecturePrs/pr4/44)
==16129==  Block was alloc'd at
==16129==    at 0x488545C: malloc (vg_replace_malloc.c:446)
==16129==    by 0x108897: main (in /home/dietpi/remclones/SystemSoftwareArchitecturePrs/pr4/44)
==16129==
==16129== Invalid write of size 8
==16129==    at 0x4892B2C: memset (vg_replace_strmem.c:1390)
==16129==    by 0x1088BF: main (in /home/dietpi/remclones/SystemSoftwareArchitecturePrs/pr4/44)
==16129==  Address 0x4a7f048 is 8 bytes inside a block of size 16 free'd
==16129==    at 0x48884F8: free (vg_replace_malloc.c:989)
==16129==    by 0x1088C7: main (in /home/dietpi/remclones/SystemSoftwareArchitecturePrs/pr4/44)
==16129==  Block was alloc'd at
==16129==    at 0x488545C: malloc (vg_replace_malloc.c:446)
==16129==    by 0x108897: main (in /home/dietpi/remclones/SystemSoftwareArchitecturePrs/pr4/44)
==16129==
==16129== Invalid free() / delete / delete[] / realloc()
==16129==    at 0x48884F8: free (vg_replace_malloc.c:989)
==16129==    by 0x1088C7: main (in /home/dietpi/remclones/SystemSoftwareArchitecturePrs/pr4/44)
==16129==  Address 0x4a7f040 is 0 bytes inside a block of size 16 free'd
==16129==    at 0x48884F8: free (vg_replace_malloc.c:989)
==16129==    by 0x1088C7: main (in /home/dietpi/remclones/SystemSoftwareArchitecturePrs/pr4/44)
==16129==  Block was alloc'd at
==16129==    at 0x488545C: malloc (vg_replace_malloc.c:446)
==16129==    by 0x108897: main (in /home/dietpi/remclones/SystemSoftwareArchitecturePrs/pr4/44)
==16129==
iteration 2, ptr = 0x4a7f040
==16129==
==16129== HEAP SUMMARY:
==16129==     in use at exit: 0 bytes in 0 blocks
==16129==   total heap usage: 2 allocs, 4 frees, 1,040 bytes allocated
==16129==
==16129== All heap blocks were freed -- no leaks are possible
==16129==
==16129== For lists of detected and suppressed errors, rerun with: -s
==16129== ERROR SUMMARY: 6 errors from 3 contexts (suppressed: 0 from 0)
```

У якості виправлення коду, обнулимо вказівник після free:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	void *ptr = NULL;
	size_t n = 16;

	for (int i = 0; i < 3; i++) {
		if (!ptr)
			ptr = malloc(n);

		printf("iteration %d, ptr = %p\n", i, ptr);

		memset(ptr, 0, n);

		free(ptr);
		ptr = NULL;
	}

	return 0;
}
```

Виконаймо повторно:

```bash
dietpi@DietPi:pr4$ make 44
cc     44.c   -o 44
dietpi@DietPi:pr4$ ./44
iteration 0, ptr = 0x558ab072a0
iteration 1, ptr = 0x558ab072a0
iteration 2, ptr = 0x558ab072a0
dietpi@DietPi:pr4$ valgrind ./44
==16167== Memcheck, a memory error detector
==16167== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==16167== Using Valgrind-3.24.0 and LibVEX; rerun with -h for copyright info
==16167== Command: ./44
==16167==
iteration 0, ptr = 0x4a7f040
iteration 1, ptr = 0x4a7f4d0
iteration 2, ptr = 0x4a7f520
==16167==
==16167== HEAP SUMMARY:
==16167==     in use at exit: 0 bytes in 0 blocks
==16167==   total heap usage: 4 allocs, 4 frees, 1,072 bytes allocated
==16167==
==16167== All heap blocks were freed -- no leaks are possible
==16167==
==16167== For lists of detected and suppressed errors, rerun with: -s
==16167== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

Як бачимо, use-after-free більше не відбувається.
Також бачимо, що при виконанні без valgrind адрес повертається однаковий. Це нормально, оскільки алокатор в нормі намагається ефективно перевикористовувати наявну пам'ять, у т.ч. щойно звільнену.

## Завдання 4.5

Якщо realloc не може виділити пам'ять, він повертає NULL і залишає попередній блок пам'яті без змін. Старий вказівник залишається валідним і повинен бути звільнений вручну. Тому безпечний спосіб використання realloc полягає у збереженні результату в тимчасовій змінній та перевірці її на NULL, щоб не втратити посилання на попередньо виділений блок пам'яті.

### Код

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
        size_t small = 1024;
        void *ptr = malloc(small);

        if (!ptr) {
                perror("malloc");
                return 1;
        }

        printf("Initial ptr = %p\n", ptr);

        // approx. 4 exabytes
        size_t huge = (size_t)1 << 62;

        void *tmp = realloc(ptr, huge);

        if (tmp == NULL) {
                printf("realloc failed, returned NULL\n");
                printf("original ptr is still valid: %p\n", ptr);

                ((char*)ptr)[0] = 'A';
                printf("write to old memory succeeded\n");

                free(ptr);
        } else {
                printf("realloc succeeded: %p\n", tmp);
                free(tmp);
        }

        return 0;
}
```

### Компіляція та запуск

```bash
dietpi@DietPi:pr4$ make realloc
cc     realloc.c   -o realloc
dietpi@DietPi:pr4$ ./realloc
Initial ptr = 0x557d1742a0
realloc failed, returned NULL
original ptr is still valid: 0x557d1742a0
write to old memory succeeded
```

## Завдання 4.6

realloc(NULL, size) працює як malloc(size).
realloc(ptr, 0) у більшості реалізацій звільняє памʼять (free(ptr)) і повертає NULL.
Після цього початковий вказівник використовувати не можна.

### Код

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
        void *a = realloc(NULL, 16);
        printf("realloc(NULL,16) = %p\n", a);

        void *b = malloc(16);
        void *r = realloc(b, 0);
        printf("realloc(b,0) = %p\n", r);

        free(a);
        return 0;
}
```

### Компіляція та запуск

```bash
dietpi@DietPi:pr4$ make 46
cc     46.c   -o 46
dietpi@DietPi:pr4$ ./46
realloc(NULL,16) = 0x55a062d2a0
realloc(b,0) = (nil)
```
