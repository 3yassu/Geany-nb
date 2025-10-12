CC = gcc
CFLAGS = -Wall -Wextra -lyyjson
all: prog

prog: json_test.o
	$(CC) $(CFLAGS) -o json_test json_test.o

json_test.o: json_test.c
	$(CC) $(CFLAGS) -c json_test.c
	
clean:
	rm -f json_test *.o
