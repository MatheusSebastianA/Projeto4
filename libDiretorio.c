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
    struct nodoM *aux = NULL;
        
    if(diretorio_vazio(d))
        return NULL;

    aux = d->inicio;

    for(int i = 0; i <= d->fim->ordem; i++){
        if(compara_nome(aux->nomeArq, nomeArq, 0)){
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
    

    free(aux);

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
        d->inicio->tam_nome = strlen(nomeArq);
        d->inicio = conteudo(d->inicio, nomeArq);
        d->inicio->ordem = 0;
        d->inicio->localizacao = sizeof(long int);
        d->inicio_diretorio = d->inicio->localizacao + d->inicio->tamanho;
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
        return NULL;

    strcpy(aux->prox->nomeArq, nomeArq);
    aux->prox->tam_nome = strlen(nomeArq);
    aux->prox = conteudo(aux->prox, nomeArq);
    aux->prox->ordem = aux->ordem + 1;
    aux->prox->localizacao = aux->localizacao + aux->tamanho;
    aux->prox->prox = NULL;

    d->inicio_diretorio = d->inicio_diretorio + aux->prox->tamanho;
    d->fim = aux->prox;

    return d->fim;
}

int insere_diretorio_apos_target(struct diretorio *d, char *nomeArq, char *target){
    struct nodoM *aux, *novo, *temp, *temp2, *anterior;
    
    aux = existe_arq(d, target);

    if(aux == NULL)
        return 1;
    
    novo = existe_arq(d, nomeArq);

    if(novo != NULL){
        
        if(aux->prox == novo)
            return 1;

        if(novo->ordem == d->fim->ordem){
            printf("TO AQUI PAE 1\n");
            temp = aux->prox;
            anterior = d->inicio;

            while(anterior->prox != novo)
                anterior = anterior->prox;
            

            anterior->prox = novo->prox;
            aux->prox = novo;
            novo->prox = temp;

            int i = 0;
            aux = d->inicio;
            while(aux->prox != NULL){
                aux->ordem = i;
                aux = aux->prox;
                i++;
            }
            aux->ordem = i;
            d->fim = aux;
            d->fim->prox = NULL;
            return 0;
        }

        if(novo->ordem == d->inicio->ordem){
            printf("TO AQUI PAE 2\n");
            d->inicio = novo->prox;
            temp = aux->prox;
            anterior = d->inicio;
            aux->prox = novo;
            novo->prox = temp;

            int i = 0;
            aux = d->inicio;
            while(aux->prox != NULL){
                aux->ordem = i;
                aux = aux->prox;
                i++;
            }

            aux->ordem = i;
            d->fim = aux;
            d->fim->prox = NULL;
        
            return 0;
        }
        
        if(novo->ordem > aux->ordem){
            printf("TO AQUI PAE 3\n");
            int ordem = novo->ordem;
            temp = aux->prox;
            aux->prox = novo;
            temp2 = novo->prox;
            novo->prox = temp;

            for(int i = temp->ordem; i < ordem - 1; i++){
                temp->ordem = temp->ordem + 1;
                temp = temp->prox;
            }

            temp->prox = temp2;
            novo->ordem = aux->ordem + 1;

            return 0;
        }

        if(novo->ordem < aux->ordem){
            printf("TO AQUI PAE 4\n");
            temp = aux->prox;
            aux->prox = novo;
            anterior = d->inicio;

            while(anterior->prox != novo){
                anterior->ordem = anterior->ordem - 1;
                anterior = anterior->prox;
            }

            anterior->prox = novo->prox;
            novo->prox = temp;

            int i = 0;
            aux = d->inicio;
            while(aux->prox != NULL){
                aux->ordem = i;
                aux = aux->prox;
                i++;
            }

            aux->ordem = i;
            d->fim = aux;
            d->fim->prox = NULL;

            return 0;
        }  

    }

    if(!(novo = malloc(sizeof(struct nodoM))))
        return 1;

    if(aux->prox == NULL){
        strcpy(novo->nomeArq, nomeArq);
        novo = conteudo(novo, nomeArq);
        novo->tam_nome = strlen(nomeArq);
        novo->ordem = aux->ordem + 1;
        novo->prox = NULL;
        aux->prox = novo;
        d->fim = novo;
        return 0;
    }

    temp = aux->prox;
    strcpy(novo->nomeArq, nomeArq);
    novo = conteudo(novo, nomeArq);
    novo->tam_nome = strlen(nomeArq);
    novo->ordem = aux->ordem + 1;
    aux->prox = novo;
    novo->prox = temp;

    return 0;
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
    printf("Inicio do Diretorio: %ld\n", d->inicio_diretorio);
    struct nodoM *aux = d->inicio;

    for(int i = 0; i <= d->fim->ordem; i++){
        printf("nome arq: %s\n", aux->nomeArq);
        printf("Tamanho do nome do arquivo: %d\n", aux->tam_nome);
        printf("Inicio do arquivo: %ld\n", aux->localizacao);
        printf("Tamanho do arquivo: %ld\n", aux->tamanho);
        printf("Ordem do arq: %d\n", aux->ordem);
        
        aux = aux->prox;
    }
    
    return;
}

void destroi_diretorio(struct diretorio *d){
    struct nodoM *aux = NULL;

    if(diretorio_vazio(d)){
        free(d->inicio);
        free(d->fim);
        free(d);
        return;
    }
    
    aux = d->inicio;
    while(aux->prox != NULL){
        aux = aux->prox;
        free(d->inicio);
        d->inicio = aux;
    }

    free(d->fim);
    free(d);

    return;
}
