CC = gcc
CFLAGS = -Wall -Wextra -ljansson -lcurl
all: prog

prog: json_test.o json_utils.o
	$(CC) $(CFLAGS) -o json_test json_test.o json_utils.o

json_test.o: json_test.c
	$(CC) $(CFLAGS) -c json_test.c

json_utils.o: json_utils.c
	$(CC) $(CFLAGS) -c json_utils.c
	
clean:
	rm -f json_test *.o
