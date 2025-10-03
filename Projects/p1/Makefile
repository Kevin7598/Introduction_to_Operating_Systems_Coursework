CC = gcc
CFLAGS = -Wall -std=c11

OBJS = main.o parser.o utility.o built_in.o error_handler.o
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)