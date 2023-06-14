#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "libArchive.h"


int main(int argc, char *argv[]){
    struct diretorio *d;
    int opt;
    while ((opt = getopt(argc, argv, "iam:xrch")) != -1) {
        switch (opt){
            case 'i':
                break;
            case 'a':
                break;
            case 'm':
                break;
            case 'x':
                break;
            case 'r':
                break;
            case 'c':
                break;
            case 'h':
                break;
            default:
                printf ("O argumento passado não é válido\n");
	            return (1);
        }		
    }

    d = cria_diretorio();
    insere_conteudo(d, "teste1.txt", "backup.txt", insereI);
    insere_conteudo(d, "teste2.txt", "backup.txt", insereI);
    insere_conteudo(d, "teste3.txt", "backup.txt", insereI);
    insere_diretorio(d, "backup.txt");

    
    extrai_conteudo_arquivo("backup.txt", "teste1.txt");


    return 0;
}
