CFLAGS= -O0 -ggdb3 -Wall
BIN=xor-ll
SRCS=xor-ll.c main.c

all: build

build:
	cc  -o $(BIN) $(SRCS) $(CFLAGS)

clean:
	rm -rf $(BIN)