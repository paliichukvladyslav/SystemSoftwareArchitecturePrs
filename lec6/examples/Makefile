CC=gcc

BASE_CFLAGS=-g -fno-omit-frame-pointer
SAN_CFLAGS=-fsanitize=address

CFLAGS=$(BASE_CFLAGS)
ifeq ($(SAN),1)
CFLAGS+=$(SAN_CFLAGS)
endif

TARGETS=heap-buffer-overflow heap-double-free heap-uaf heap-leak stack-buffer-overflow

all:$(TARGETS)

%:%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)
