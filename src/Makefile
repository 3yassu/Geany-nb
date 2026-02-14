CC = gcc
CFLAGS = -Wall -Wextra -lyyjson
PCFLAGS = $(shell python3-config --includes --cflags)
PLFLAGS = $(shell python3-config --ldflags --embed)
GOFLAGS = -shared `pkg-config --libs geany`
GCFLAGS = -fPIC `pkg-config --cflags geany`

all: geany-nb

geany-nb: geany-nb.o notebook.o python.o
	$(CC) $(CFLAGS) -o ~/.config/geany/plugins/geany-nb.so $^ $(GOFLAGS) $(PLFLAGS)

geany-nb.o: geany-nb.c
	$(CC) $(CFLAGS) -c geany-nb.c $(GCFLAGS)

notebook.o: json/notebook.c
	$(CC) $(CFLAGS) -c json/notebook.c -fPIC

python.o: interpreter/python.c
	$(CC) $(CFLAGS) -c interpreter/python.c -fPIC $(PCFLAGS)

clean:
	rm -f geany-nb.so *.o
