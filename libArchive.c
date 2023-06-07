#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "libArchive.h"

#define BUFFER_SIZE 1024

FILE* abre_archive_escrita(char *nomeArq){
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
int insere_conteudo_menor1024(struct nodoM *nodo, FILE *arq, FILE *archive){
    char buffer[BUFFER_SIZE];
    int tam = nodo->tamanho % BUFFER_SIZE;

    for(int i = 0; i < tam; i++)
        fread(&buffer[i], sizeof(char), 1, arq);
    
    fwrite(&buffer, sizeof(char), tam, archive);

    return 0;
}

int insere_bloco_conteudo(struct nodoM *nodo, FILE *arq, FILE *archive){
    char buffer[BUFFER_SIZE];

    fread(buffer, sizeof(char), BUFFER_SIZE, arq);
    fwrite(buffer, sizeof(char), BUFFER_SIZE, archive);

    return 0;
}

int insere_conteudo(struct diretorio *d, char *nomeArq, char *nomeArc, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq)){
    int blocos, resto, bytes = 0;
    FILE *arq, *arc;
    struct nodoM *aux;

    arq = abre_archive_leitura(nomeArq);
    if(!arq)
        return 1;
    
    arc = abre_archive_escrita(nomeArc);   
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

    fwrite(&d->inicio_diretorio, sizeof(long int), 1, arc);
    aux = malloc(sizeof(struct nodoM));
    aux = d->inicio;
    while(aux->prox != NULL){
        bytes = bytes + aux->tamanho;
        aux = aux->prox;
    }
    insere(d, nomeArq, func);
    bytes = aux->tamanho + bytes;
    fseek(arc, bytes, SEEK_SET);
    blocos = d->fim->tamanho / BUFFER_SIZE;
    resto = d->fim->tamanho % BUFFER_SIZE;
    if(blocos >= 1)
        for(blocos = blocos; blocos > 0; blocos--)
            insere_bloco_conteudo(d->fim, arq, arc);

    if(blocos < 1 && resto != 0)
        insere_conteudo_menor1024(d->fim, arq, arc);

    fwrite(d, sizeof(struct diretorio), 1, arc);
    printf("Esse numero é o inicio do dir: %ld\n", d->inicio_diretorio);

    fclose(arq);
    fclose(arc);
    
    return 0;
}   




