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
    arq = fopen(nomeArq, "r+b");

    if(arq == NULL)
        arq = fopen(nomeArq, "wb");

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

struct diretorio* recebe_diretorio(struct diretorio *d, char *nomeArc){
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
        if(!(aux = malloc(sizeof(struct nodoM))))
            return NULL;

        fread(&aux->tam_nome, sizeof(int), 1, arc);
        fread(aux->nomeArq,sizeof(char), aux->tam_nome, arc);
        fread(&aux->data, sizeof(time_t), 1, arc);
        fread(&aux->localizacao, sizeof(long int), 1, arc);
        fread(&aux->ordem, sizeof(int), 1, arc);
        fread(&aux->permissoes, sizeof(mode_t), 1, arc);
        fread(&aux->tamanho, sizeof(off_t), 1, arc);
        fread(&aux->uid, sizeof(uid_t), 1, arc);

        if(diretorio_vazio(d)){

            if(!(d->inicio = malloc(sizeof(struct nodoM))))
                return NULL;
            d->inicio = aux;
            d->inicio->prox = NULL;
            d->inicio_diretorio = 0;
            d->fim = d->inicio;
        }

        else{
            if(!(d->fim->prox = malloc(sizeof(struct nodoM))))
                return NULL;
            d->fim->prox = aux;
            d->fim = aux;
            d->inicio_diretorio = 0;
        }


        aux = aux->prox;
    }
    fclose(arc);

    return d;
}

void insere_diretorio(struct diretorio *d, char *nomeArc){
    struct nodoM *aux = NULL;
    FILE *arc = abre_archive_leitura_escrita(nomeArc);   

    if(!arc)
        return;
        
    aux = d->inicio;
    fseek(arc, d->inicio_diretorio, SEEK_SET);

    for(int i = 0; i <= d->fim->ordem; i++){
        fwrite(&aux->tam_nome, sizeof(int), 1, arc);
        fwrite(aux->nomeArq, strlen(aux->nomeArq), 1, arc);
        fwrite(&aux->data, sizeof(time_t), 1, arc);
        fwrite(&aux->localizacao, sizeof(long int), 1, arc);
        fwrite(&aux->ordem, sizeof(int), 1, arc);
        fwrite(&aux->permissoes, sizeof(mode_t), 1, arc);
        fwrite(&aux->tamanho, sizeof(off_t), 1, arc);
        fwrite(&aux->uid, sizeof(uid_t), 1, arc);

        aux = aux->prox;
    }

    fclose(arc);

    return;
}

int insere_conteudo_menor1024(struct nodoM *nodo, FILE *arq, FILE *archive){
    char buffer[BUFFER_SIZE] = {0};
    int tam = 0;
    int blocos = nodo->tamanho / BUFFER_SIZE;
    tam = nodo->tamanho % BUFFER_SIZE;
    
    for(int i = 0; i < tam; i++)
        fread(&buffer[i], sizeof(char), 1, arq);

    fseek(archive, nodo->localizacao + (blocos * 1024), SEEK_SET);
    fwrite(&buffer, sizeof(char), tam, archive);    

    return 0;
}

int insere_bloco_conteudo(struct nodoM *nodo, FILE *arq, FILE *archive){
    char buffer[BUFFER_SIZE] = {0};

    fread(buffer, sizeof(char), BUFFER_SIZE, arq);
    fwrite(buffer, sizeof(char), BUFFER_SIZE, archive);

    return 0;
}

int insere_conteudo(struct diretorio *d, char *nomeArq, char *nomeArc, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq)){
    int blocos = 0, resto = 0;
    FILE *arq = NULL, *arc = NULL;
    struct nodoM *aux = NULL;

    arq = abre_archive_leitura(nomeArq);
    if(!arq)
        return 1;
    
    arc = abre_archive_leitura_escrita(nomeArc);   
    if(!arc)
        return 1;

    if(diretorio_vazio(d)){
        d->inicio = insere(d, nomeArq, func);
        fseek(arc, 0, SEEK_SET);
        fwrite(&d->inicio_diretorio, sizeof(long int), 1, arc);
        blocos = d->inicio->tamanho / BUFFER_SIZE;
        resto = d->inicio->tamanho % BUFFER_SIZE;
        if(blocos >= 1)
            for(blocos = blocos; blocos > 0; blocos--)
                insere_bloco_conteudo(d->inicio, arq, arc);

        fseek(arq, 1024*blocos, SEEK_SET);
        if(blocos < 1 && resto != 0)
            insere_conteudo_menor1024(d->inicio, arq, arc);
        
        fclose(arq);
        fclose(arc);
        return 0;
    }
    
    insere(d, nomeArq, func);
    aux = d->fim;
    fseek(arc, 0, SEEK_SET);
    fwrite(&d->inicio_diretorio, sizeof(long int), 1, arc);
    fseek(arc, d->inicio_diretorio, SEEK_SET);
    blocos = aux->tamanho / BUFFER_SIZE;
    resto = aux->tamanho % BUFFER_SIZE;
    if(blocos >= 1)
        for(blocos = blocos; blocos > 0; blocos--)
            insere_bloco_conteudo(aux, arq, arc);

    if(blocos < 1 && resto != 0)
        insere_conteudo_menor1024(aux, arq, arc);
    

    fclose(arq);
    fclose(arc);
    
    return 0;
}

void extrai_conteudo_arquivo(char *arc, char *dest){
    FILE *archive, *destino;
    struct nodoM *aux = NULL;
    struct diretorio *b = cria_diretorio();
    recebe_diretorio(b, "backup.txt");
    imprime_diretorio(b);

    aux = existe_arq(b, dest);
    if(aux == NULL)
        return;

    archive = abre_archive_leitura_escrita(arc);
    destino = abre_archive_leitura_escrita(dest);

    fseek(archive, aux->localizacao, SEEK_SET);
    aux->localizacao = 0;
    aux->tamanho = aux->tamanho;
    int blocos = aux->tamanho / BUFFER_SIZE;
    int resto = aux->tamanho % BUFFER_SIZE;
    if(blocos >= 1)
        for(blocos = blocos; blocos > 0; blocos--)
            insere_bloco_conteudo(aux, archive, destino);

    if(blocos < 1 && resto != 0)
        insere_conteudo_menor1024(aux, archive, destino);

    return;
}

void atualiza_conteudo(struct diretorio *d, char *nomeArq, char *nomeArc){
    char buffer[BUFFER_SIZE] = {0};
    FILE *archive, *arquivo;
    struct nodoM *aux = NULL;
    int i = 0, blocos, resto;
    long int tam_ant_arq = 0, diferenca = 0, fim = 0, teste = 0;

    aux = existe_arq(d, nomeArq);
    if(aux == NULL)
        return;

    tam_ant_arq = aux->tamanho;
    conteudo(aux, nomeArq);
    diferenca = aux->tamanho - tam_ant_arq;

    archive = abre_archive_leitura_escrita(nomeArc);
    if(archive == NULL)
        return;
    
    fseek(archive, 0, SEEK_END);
    fim = ftell(archive);
    rewind(archive);
    fread(&d->inicio_diretorio, sizeof(long int), 1, archive);
    fseek(archive, aux->localizacao + tam_ant_arq, SEEK_SET);

    if(diferenca > 0){
        teste = ftell(archive);
        blocos = (d->inicio_diretorio - ftell(archive)) / BUFFER_SIZE;
        resto = (d->inicio_diretorio - ftell(archive)) % BUFFER_SIZE;
        if(blocos >= 1)
            for(blocos = blocos; blocos > 0; blocos--){
                fread(buffer, sizeof(char), BUFFER_SIZE, archive);
                fseek(archive, diferenca - BUFFER_SIZE, SEEK_CUR);
                fwrite(buffer, sizeof(char), BUFFER_SIZE, archive);
            }
        if(blocos < 1 && resto != 0){
            for(i = 0; i < resto; i++){
                fread(&buffer[i], sizeof(char), 1, archive);
            }
            
            fseek(archive, aux->localizacao + aux->tamanho + (blocos * 1024), SEEK_SET);
            fwrite(&buffer, sizeof(char), resto, archive);
            

        }
            
    }
    arquivo = abre_archive_leitura(nomeArq);

    if(!arquivo)
        return;

    blocos = aux->tamanho / BUFFER_SIZE;
    resto = aux->tamanho % BUFFER_SIZE;
    fseek(archive, aux->localizacao, SEEK_CUR);
    if(blocos >= 1)
        for(blocos = blocos; blocos > 0; blocos--)
            insere_bloco_conteudo(aux, arquivo, archive);

    if(blocos < 1 && resto != 0)
        insere_conteudo_menor1024(aux, arquivo, archive);

    
    for(int i = aux->ordem; i <= d->fim->ordem - 1; i++){
        aux->prox->localizacao = aux->prox->localizacao + diferenca;
            
        aux = aux->prox;
    }
    printf("Depois das att:\n");
    imprime_diretorio(d);
    d->inicio_diretorio = d->inicio_diretorio + diferenca;
    rewind(archive);
    fwrite(&d->inicio_diretorio, sizeof(long int), 1, archive);
    insere_diretorio(d, nomeArc);

    fclose(archive);
    fclose(arquivo);

    return;
}
