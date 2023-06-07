#ifndef libArchive
#define libArchive

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "libDiretorio.h"

FILE* abre_archive_escrita(char *nomeArq);

FILE* abre_archive_leitura(char *nomeArq);

void insere_diretorio(struct diretorio *d, char *nomeArc);

int insere_conteudo_menor1024(struct nodoM *nodo, FILE *arq, FILE *archive);

int insere_bloco_conteudo(struct nodoM *nodo, FILE *arq, FILE *archive);

int insere_conteudo(struct diretorio *d, char *nomeArq, char *nomeArc, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq));

void imprime_conteudo(char *nomeArq);



#endif /* libArchive.h*/
