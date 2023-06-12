#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libDiretorio.h"

struct diretorio* cria_diretorio(){
    struct diretorio *d;

    if(!(d = malloc(sizeof(struct diretorio))))
        return NULL;

    d->inicio = NULL;
    d->fim = NULL;
    d->inicio_diretorio = 0;
    
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
    struct nodoM *aux;
        
    if(diretorio_vazio(d))
        return NULL;

    aux = d->inicio;

    for(int i = 0; i <= d->fim->ordem; i++){
        if(compara_nome(aux->nomeArq, nomeArq, 0)){
            printf("Já existe paizao\n");
            return aux;
        }
        aux = aux->prox;
    }

    return NULL;
}

struct nodoM* conteudo(struct nodoM *nodo, char *nomeArq){
    struct stat *aux;
    if(!(aux = malloc(sizeof(struct stat))))
        return NULL;

    stat(nomeArq, aux);
    nodo->data = aux->st_mtime;
    nodo->permissoes = aux->st_mode;
    nodo->tamanho = aux->st_size;
    nodo->uid = aux->st_uid;
    
    return nodo;
}

struct nodoM* insereI(struct nodoM *nodo, char *nomeArq){
    nodo = conteudo(nodo, nomeArq);

    return nodo;
}

struct nodoM* insereA(struct nodoM *nodo, char *nomeArq){
    struct stat *compara;
    compara = malloc(sizeof(struct stat));
    stat(nomeArq, compara);

    if(compara->st_mtime <= nodo->data)
        nodo = conteudo(nodo, nomeArq);

    return nodo;
}

struct nodoM* insere(struct diretorio *d, char *nomeArq, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq)){
    struct nodoM *aux = NULL;
    
    if(diretorio_vazio(d)){
        if(!(d->inicio =  malloc(sizeof(struct nodoM))))
            return NULL;
        strcpy(d->inicio->nomeArq, nomeArq);
        d->inicio = conteudo(d->inicio, nomeArq);
        d->inicio->ordem = 0;
        d->inicio->localizacao = sizeof(long int);
        d->inicio_diretorio = d->inicio->tamanho + d->inicio->localizacao - 1;
        d->fim = d->inicio;
        d->fim->prox = NULL;

        return d->inicio;
    }

    aux = existe_arq(d, nomeArq);
    if(aux != NULL){
        aux = func(aux, nomeArq);
        return aux;
    }

    aux = d->fim;
    if(!(aux->prox = malloc(sizeof(struct nodoM))))
        return 0;

    strcpy(aux->prox->nomeArq, nomeArq);
    aux->prox = conteudo(aux->prox, nomeArq);
    aux->prox->ordem = aux->ordem + 1;
    aux->prox->localizacao = d->inicio_diretorio;
    d->inicio_diretorio = d->inicio_diretorio + aux->prox->tamanho - 1;
    
    d->fim = aux->prox;

    return d->fim;
}

int insere_apos_target(struct diretorio *d, char *nomeArq, char *target){
    struct nodoM *aux, *novo, *temp;
    
    aux = existe_arq(d, target);

    if(aux == NULL)
        return 0;

    if(!(novo = malloc(sizeof(struct nodoM))))
        return 0;

    if(aux->prox == NULL){
        strcpy(novo->nomeArq, nomeArq);
        novo = conteudo(novo, nomeArq);
        novo->ordem = aux->ordem + 1;
        novo->prox = NULL;
        aux->prox = novo;
        d->fim = novo;
        return 1;
    }

    temp = aux->prox;
    strcpy(novo->nomeArq, nomeArq);
    novo = conteudo(novo, nomeArq);
    novo->ordem = aux->ordem + 1;
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
        printf("Diretorio vazio\n");
        return;
    }
    printf("Inicio do Diretorio fdp: %ld\n", d->inicio_diretorio);
    struct nodoM *aux = d->inicio;

    for(int i = 0; i <= d->fim->ordem; i++){
        printf("nome arq: %s\n", aux->nomeArq);
        printf("Inicio do arquivo: %ld\n", aux->localizacao);
        printf("Tamanho do arquivo: %ld\n", aux->tamanho);
        printf("Ordem do arq: %d\n", aux->ordem);
        
        aux = aux->prox;
    }
    
    return;
}
