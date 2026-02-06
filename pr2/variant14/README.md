# Практична робота №2
## Завдання за варіантами (14)

```
14. Реалізуйте програму для аналізу вмісту /proc/self/maps.
```
### maps

Для виконання цього завдання було створено інструмент **maps**, який читає вміст /proc/self/maps порядково за допомогою fgets, парсить його використовуючи sscanf та заповнюючи масив з інформацією для кожного окремого мапінгу. Після цього перед користувачем предстає детальний аналіз мапінгів (напр. їх розмір), форматований за допомогою ANSI Escape кодів.
### Компіляція та запуск

```bash
CFLAGS=-Wall make maps
./maps
```

Приклад виводу:

```
mapping for /tmp/maps with offset 0 with r--p perms
starts at 556eb715b000, ends at 556eb715c000, total 4096 bytes
device 0:33, inode number 175
================================================================================
mapping for /tmp/maps with offset 1000 with r-xp perms
starts at 556eb715c000, ends at 556eb715d000, total 4096 bytes
device 0:33, inode number 175
================================================================================
mapping for /tmp/maps with offset 2000 with r--p perms
starts at 556eb715d000, ends at 556eb715e000, total 4096 bytes
device 0:33, inode number 175
================================================================================
...
```
