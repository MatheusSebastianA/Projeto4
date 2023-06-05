#ifndef libArchive
#define libArchive

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "libDiretorio.h"

struct blocoConteudo{
    struct blocoConteudo *prox;
    char buffer[1024];
};

struct archive{
    int pDiretorio;
    struct blocoConteudo *inicioArc;
    struct diretorio *dir;
};

int archive_vazio(struct archive *arc);

struct archive* cria_archive();

struct archive*  insere_conteudo(struct archive *arc, char *nomeArq);

void imprime_conteudo(struct archive *arc, char *nomeArq);



#endif /* libArchive.h*/
