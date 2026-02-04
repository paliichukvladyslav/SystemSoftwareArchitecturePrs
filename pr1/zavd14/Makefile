CC=gcc
CFLAGS=-Wall

SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:.c=.o)

BIN_NAME=last

default: $(BIN_NAME)

$(BIN_NAME): $(OBJS)
	$(CC) $(OBJS) -o $(BIN_NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
