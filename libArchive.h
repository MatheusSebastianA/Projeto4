#ifndef libArchive
#define libArchive

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "libDiretorio.h"

FILE* abre_archive_leitura_escrita(char *nomeArq);

FILE* abre_archive_leitura(char *nomeArq);

struct diretorio* recebe_diretorio(struct diretorio *d, char *nomeArc);

void insere_diretorio(struct diretorio *d, char *nomeArc);

int insere_conteudo_menor1024(struct nodoM *nodo, FILE *arq, FILE *archive);

int insere_bloco_conteudo(struct nodoM *nodo, FILE *arq, FILE *archive);

int insere_conteudo(struct diretorio *d, char *nomeArq, char *nomeArc, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq));

void insere_conteudo_apos_target(struct diretorio *d, char *nomeArq, char *target,  char *nomeArc);

char* nome_diretorios(char nome[TAM_NOME], char *copia);

void extrai_conteudo_arquivo(struct diretorio *d, char *arc, char *dest);

void remove_conteudo(struct diretorio *d, char *arc, char *arq);

void atualiza_conteudo(struct diretorio *d, char *nomeArq, char *nomeArc, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq));

void imprime_permissoes(mode_t mode);

void imprime_informacoes(struct diretorio *d, char *nomeArc);

void imprime_opcoes();

#endif /* libArchive.h*/
