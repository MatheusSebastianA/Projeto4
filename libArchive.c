#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "libArchive.h"

#define _XOPEN_SOURCE_EXTENDED 1
#define BUFFER_SIZE 1024

FILE* abre_archive_leitura_escrita(char *nomeArq){
    FILE *arq;
    arq = fopen(nomeArq, "r+");

    if(arq == NULL)
        arq = fopen(nomeArq, "w");

    if(arq == NULL)
        return NULL;
    
    return arq;
}

FILE* abre_archive_leitura(char *nomeArq){
    FILE *arq;
    arq = fopen(nomeArq, "r");

    if(arq == NULL)
        return NULL;
    
    return arq;
}

struct diretorio* recebe_diretorio(struct diretorio *d, char *nomeArc,  struct nodoM* (* func) (struct nodoM *aux, char *nomeArq)){
    long int ini_dir = 0, fim = 0;
    struct nodoM *aux = NULL;
    FILE *arc = NULL;
    arc = abre_archive_leitura_escrita(nomeArc);   

    if(!arc)
        return NULL;

    fread(&ini_dir, sizeof(long int), 1, arc);
    fseek(arc, 0, SEEK_END);
    fim = ftell(arc);
    fseek(arc, ini_dir, SEEK_SET);
    
    while(fim - ftell(arc) != 0){
        aux = malloc(sizeof(struct nodoM));
        fread(aux, sizeof(struct nodoM), 1, arc);
        insere(d, aux->nomeArq, func);
        aux = aux->prox;
    }

    fclose(arc);

    return d;
}

void insere_diretorio(struct diretorio *d, char *nomeArc){
    int ini_dir = 0;
    struct nodoM *aux = NULL;
    FILE *arc = abre_archive_leitura_escrita(nomeArc);   

    if(!arc)
        return;
    fread(&ini_dir, sizeof(long int), 1, arc);
    fseek(arc, -1, SEEK_END);

    aux = d->inicio->prox;
        fwrite(aux, sizeof(struct nodoM), 1, arc);

    fclose(arc);
    return;
}

int insere_conteudo_menor1024(struct nodoM *nodo, FILE *arq, FILE *archive){
    char buffer[BUFFER_SIZE] = {0};
    int tam = nodo->tamanho % BUFFER_SIZE;

    for(int i = 0; i < tam; i++)
        fread(&buffer[i], sizeof(char), 1, arq);

    fseek(archive, -1, SEEK_END);
    for(int i = 0; i < tam; i++)
        fwrite(&buffer[i], sizeof(char), 1, archive);    

    return 0;
}

int insere_bloco_conteudo(struct nodoM *nodo, FILE *arq, FILE *archive){
    char buffer[BUFFER_SIZE] = {0};

    for(int i = 0; i < 1024; i++)
        fread(&buffer[i], sizeof(char), 1, arq);
    fread(buffer, sizeof(char), BUFFER_SIZE, arq);
    fwrite(buffer, sizeof(char), BUFFER_SIZE, archive);

    return 0;
}

int insere_conteudo(struct diretorio *d, char *nomeArq, char *nomeArc, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq)){
    int blocos = 0, resto = 0;
    FILE *arq = NULL, *arc = NULL;

    arq = abre_archive_leitura(nomeArq);
    if(!arq)
        return 1;
    
    arc = abre_archive_leitura_escrita(nomeArc);   
    if(!arc)
        return 1;

    if(diretorio_vazio(d)){
        insere(d, nomeArq, func);
        fwrite(&d->inicio_diretorio, sizeof(long int), 1, arc);
        blocos = d->inicio->tamanho / BUFFER_SIZE;
        resto = d->inicio->tamanho % BUFFER_SIZE;
        if(blocos >= 1)
            for(blocos = blocos; blocos > 0; blocos--)
                insere_bloco_conteudo(d->inicio, arq, arc);

        if(blocos < 1 && resto != 0)
            insere_conteudo_menor1024(d->inicio, arq, arc);
        
        fclose(arq);
        fclose(arc);
        return 0;
    }
    
    insere(d, nomeArq, func);
    fseek(arc, 0, SEEK_SET);
    fwrite(&d->inicio_diretorio, sizeof(long int), 1, arc);
    fseek(arc, d->fim->localizacao + d->fim->tamanho - 1, SEEK_SET);
    blocos = d->fim->tamanho / BUFFER_SIZE;
    resto = d->fim->tamanho % BUFFER_SIZE;
    if(blocos >= 1)
        for(blocos = blocos; blocos > 0; blocos--)
            insere_bloco_conteudo(d->fim, arq, arc);

    if(blocos < 1 && resto != 0)
        insere_conteudo_menor1024(d->fim, arq, arc);
    

    fclose(arq);
    fclose(arc);
    
    return 0;
}
