CC = gcc
CFLAGS = -Wall -Wextra $(shell python-config --cflags --ldflags --embed) 
all: prog

prog: test.o
	$(CC) $(CFLAGS) -o pytest test.o

test.o: test.c
	$(CC) $(CFLAGS) -c test.c
	
clean:
	rm -f main.py *.o
