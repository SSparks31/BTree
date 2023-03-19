CC = gcc
CFLAGS = -std=c99 -fstack-protector-all -Wall

.PHONY: test clean	

trab: main.c BTree.o
	$(CC) $(CFLAGS) -g -o $@ $^

test: trab
	./trab

BTree.o: BTree/BTree.c BTree/BTree.h
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -rf *.o
	-rm ted