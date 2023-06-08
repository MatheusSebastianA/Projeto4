#ifndef libDiretorio
#define libDiretorio

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define TAM_NOME 64

struct nodoM{
    char nomeArq[TAM_NOME];
    uid_t uid;
    mode_t permissoes;
    off_t tamanho;
    __time_t data;
    struct nodoM *prox; 
    int ordem;
    long int localizacao;
};

struct diretorio{
    struct nodoM *inicio;
    struct nodoM *fim;
    long int inicio_diretorio;
};

struct diretorio* cria_diretorio();

int compara_caractere(char c1, char c2);

int compara_nome(char *s1, char *s2, int cont );

struct nodoM* existe_arq(struct diretorio *d, char *nomeArq);

struct nodoM* conteudo(struct nodoM *nodo, char *nomeArq);

struct nodoM* insereI(struct nodoM *nodo, char *nomeArq);

struct nodoM* insereA(struct nodoM *nodo, char *nomeArq);

struct nodoM* insere(struct diretorio *d, char *nomeArq, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq));

int insere_apos_target(struct diretorio *d, char *nomeArq, char *target);

int diretorio_vazio(struct diretorio *d);
    
void imprime_diretorio(struct diretorio *d);


#endif /* libDiretorio.h*/
