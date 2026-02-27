CC=gcc
CFLAGS=-g -fno-omit-frame-pointer -fsanitize=address
TARGETS=heap-buffer-overflow heap-double-free heap-uaf heap-leak stack-buffer-overflow

all:$(TARGETS)

%:%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)
