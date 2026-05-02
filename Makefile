CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic

OBJ = obj
BIN = bin

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,$(OBJ)/%.o, $(SRCS))

.PHONY: all clean

all: proj2

proj2: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ)/%.o: %.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir -p $@

clean:
	rm -rf $(OBJ)

