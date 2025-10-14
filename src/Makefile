CC = gcc
CFLAGS = -Wall -Wextra -lyyjson -luuid

all: geany-nb

geany-nb: geany-nb.o json-parser.o
	$(CC) $(CFLAGS) -o ~/.config/geany/plugins/geany-nb.so geany-nb.o json-parser.o -shared `pkg-config --libs geany`

geany-nb.o: geany-nb.c
	$(CC) $(CFLAGS) -c geany-nb.c -fPIC `pkg-config --cflags geany`

json-parser.o: json-parser.c
	$(CC) $(CFLAGS) -c json-parser.c -fPIC `pkg-config --cflags geany`

clean:
	rm -f geany-nb.so *.o
