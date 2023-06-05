#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "libArchive.h"

#define CONT_TAM 1024

struct archive* cria_archive(){
    struct archive *arc;

    if(!(arc = malloc(sizeof(struct archive))))
        return NULL;


    arc->pDiretorio = 0;
    arc->dir = cria_diretorio();
    arc->inicioArc = NULL;

    return arc;
}

int archive_vazio(struct archive *arc){
    if(arc->inicioArc == NULL)
        return 1;

    return 0;
}

struct archive* insere_conteudo(struct archive *arc, char *nomeArq){
    FILE *arq;
    arq = fopen(nomeArq, "r");

    if(archive_vazio(arc)){
        if(!(arc->inicioArc = malloc(sizeof(struct blocoConteudo))))
            return NULL;

        struct blocoConteudo *aux;
        aux = arc->inicioArc;
        while(!feof(arq)){
            fread(aux, sizeof(char), 1024, arq);
            aux = aux->prox;
            if(!(aux = malloc(sizeof(struct blocoConteudo))))
                return NULL;
            aux->prox = NULL;
            arc->pDiretorio++;
        }

    }

    return arc;
}

void imprime_conteudo(struct archive *arc, char *nomeArq){
    FILE *arq;
    arq = fopen(nomeArq, "w");
    struct blocoConteudo *aux;
    aux = arc->inicioArc;

    fwrite(aux->buffer, sizeof(char), 1024, arq);

    return;
}

