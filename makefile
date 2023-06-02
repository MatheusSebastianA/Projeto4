CFLAGS = -Wall -std=c99  # flags de compilacao

CC = gcc

# arquivos-objeto
	objects = vina++.o libDiretorio.o

all: vina++
     
vina++: vina++.o libDiretorio.o
	$(CC) -o vina++ vina++.o libDiretorio.o

libDiretorio.o: libDiretorio.c
	$(CC) -c $(CFLAGS) libDiretorio.c

vina++.o: vina++.c
	$(CC) -c $(CFLAGS) vina++.c

clean:
	rm -f $(objects) 

purge:
	rm -f vina++
