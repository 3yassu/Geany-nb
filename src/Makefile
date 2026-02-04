CC = gcc
CFLAGS = -Wall -Wextra -lyyjson
GOFLAGS = -shared `pkg-config --libs geany`
GCFLAGS = -fPIC `pkg-config --cflags geany`

all: geany-nb

geany-nb: geany-nb.o notebook.o
	$(CC) $(CFLAGS) -o ~/.config/geany/plugins/geany-nb.so $^ $(GOFLAGS)

geany-nb.o: geany-nb.c
	$(CC) $(CFLAGS) -c geany-nb.c $(GCFLAGS)

notebook.o: json/notebook.c
	$(CC) $(CFLAGS) -c json/notebook.c $(GCFLAGS)

clean:
	rm -f geany-nb.so *.o
