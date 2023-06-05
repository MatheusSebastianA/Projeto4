CFLAGS = -Wall -std=c99  # flags de compilacao

CC = gcc

# arquivos-objeto
	objects = vina++.o libArchive.o libDiretorio.o 

all: vina++
     
vina++: vina++.o libDiretorio.o libArchive.o
	$(CC) -o vina++ vina++.o libArchive.o libDiretorio.o

libArchive.o: libArchive.c
	$(CC) -c $(CFLAGS) libArchive.c

libDiretorio.o: libDiretorio.c
	$(CC) -c $(CFLAGS) libDiretorio.c

vina++.o: vina++.c
	$(CC) -c $(CFLAGS) vina++.c

clean:
	rm -f $(objects) 

purge:
	rm -f vina++
