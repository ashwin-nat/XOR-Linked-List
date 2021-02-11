CFLAGS= -O0 -ggdb3 -Wall -fsanitize=address
BIN=xor-ll
SRCS=xor-ll.c example.c

all: build

build:
	cc  -o $(BIN) $(SRCS) $(CFLAGS)

clean:
	rm -rf $(BIN)