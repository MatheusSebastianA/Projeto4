#ifndef libArquivo
#define libArquivo

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
    int *pDiretorio;
    struct blocoConteudo *inicioArc;
    struct diretorio dir;
};


#endif /* libArquivo.h*/
