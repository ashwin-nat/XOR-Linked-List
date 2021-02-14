CFLAGS= -O2
CFLAGS_DBG= -O0 -ggdb3 -Wall -DXOR_LL_DEBUG
BIN=xor-ll
SRCS=xor-ll.c example.c

all: build

build:
	$(CC) -o $(BIN) $(SRCS) $(CFLAGS)

debug:
	$(CC) -o $(BIN) $(SRCS) $(CFLAGS_DBG)

clean:
	rm -rf $(BIN)