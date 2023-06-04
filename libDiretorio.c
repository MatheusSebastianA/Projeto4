#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "libDiretorio.h"


#define CONT_TAM 1024

struct diretorio* cria_diretorio(){
    struct diretorio *d;

    if(!(d = malloc(sizeof(struct diretorio))))
        return NULL;

    d->inicio = NULL;
    d->fim = NULL;
    
    return d;
}

int compara_caractere(char c1, char c2){
    if(c1 == c2)
        return 1;

    return 0;
}

int compara_nome(char *s1, char *s2, int cont){
    int tamanho_s1 = strlen(s1);
    int tamanho_s2 = strlen(s2);

    if(tamanho_s1 == tamanho_s2)
        for(int i = 0; i < tamanho_s1; i++){
            if(compara_caractere(*(s1+i), *(s2+i)))
                cont++;
        }

    if(cont == tamanho_s1)
        return 1;

    return 0;
}

struct nodoM* existe_arq (struct diretorio *d, char *nomeArq){
    struct nodoM *aux = NULL;

    if(diretorio_vazio(d))
        return NULL;

    aux = d->inicio;
    while(aux != NULL){
        if(compara_nome(aux->nomeArq, nomeArq, 0))
            return aux;

        aux = aux->prox;
    }

    return NULL;
}

struct nodoM* insereI(struct nodoM *aux, char *nomeArq){
    stat(nomeArq, aux->arquivo);
    return aux;
}

struct nodoM* insereA(struct nodoM *aux, char *nomeArq){
    struct stat *compara;
    compara = malloc(sizeof(struct stat));
    stat(nomeArq, compara);

    if(compara->st_mtime <= aux->arquivo->st_mtime)
        stat(nomeArq, aux->arquivo);

    return aux;
}

int insere(struct diretorio *d, char *nomeArq, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq)){
    struct nodoM *aux;

    if(diretorio_vazio(d)){
        if(!(d->inicio =  malloc(sizeof(struct nodoM))))
            return 0;

        d->inicio->nomeArq = nomeArq;
        if(!(d->inicio->arquivo = malloc(sizeof(struct stat))))
            return 0;

        stat(nomeArq, d->inicio->arquivo);
        d->fim = d->inicio;

        return 1;
    }

    aux = existe_arq(d, nomeArq);
    if(aux != NULL){
        aux = func(aux, nomeArq);
        return 1;
    }

    aux = d->fim;
    if(!(aux->prox = malloc(sizeof(struct nodoM))))
        return 0;

    aux->prox->nomeArq = nomeArq;

    if(!(aux->prox->arquivo = malloc(sizeof(struct stat))))
            return 0;
        
    stat(nomeArq, aux->prox->arquivo);
        d->fim = aux->prox;

    return 1;
}

int insere_apos_target(struct diretorio *d, char *nomeArq, char *target){
    struct nodoM *aux, *novo, *temp;
    
    aux = existe_arq(d, target);

    if(aux == NULL)
        return 0;

    if(!(novo = malloc(sizeof(struct nodoM))))
        return 0;
    if(!(novo->arquivo = malloc(sizeof(struct stat))))
        return 0;

    if(aux->prox == NULL){
        novo->nomeArq = nomeArq;
        stat(nomeArq, novo->arquivo);
        novo->prox = NULL;
        aux->prox = novo;
        d->fim = novo;
        return 1;
    }

    temp = aux->prox;
    novo->nomeArq = nomeArq;
    stat(nomeArq, novo->arquivo);
    aux->prox = novo;
    novo->prox = temp;

    return 1;
}

int diretorio_vazio(struct diretorio *d){
    if(d->inicio == NULL)
        return 1;

    return 0;
}

void imprime_diretorio(struct diretorio *d){
    if(diretorio_vazio(d)){
        printf("Diretorio vazio");
        return;
    }

    struct nodoM *aux = d->inicio;
    while(aux != NULL){
        printf("nome arq: %s\n", aux->nomeArq);
        printf("tamanho desse arq: %ld\n", aux->arquivo->st_size);

        aux = aux->prox;
    }
    
    return;
}
