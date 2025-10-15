CC = gcc
CFLAGS = -Wall -std=c11

OBJS = src/main.o src/parser.o src/utility.o src/built_in.o src/error_handler.o
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)