#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libDiretorio.h"

/*Função que cria a estrutura diretorio */
struct diretorio* cria_diretorio(){
    struct diretorio *d;

    if(!(d = malloc(sizeof(struct diretorio))))
        return NULL;

    d->inicio = NULL;
    d->fim = NULL;
    d->inicio_diretorio = 0;
    
    return d;
}

/*Função que compara dois caracteres */
int compara_caractere(char c1, char c2){
    if(c1 == c2)
        return 1;

    return 0;
}

/*Função que compara dois nomes */
int compara_nome(char *s1, char *s2, int cont){
    int tamanho_s1 = strlen(s1);
    int tamanho_s2 = strlen(s2);

    if(tamanho_s1 - 1 == tamanho_s2){
        if(nome_barra(s1) != 1)
            return 0;

        for(int i = 0; i < tamanho_s2; i++){
            if((compara_caractere(*(s1+i+1), *(s2+i))))
                cont++;
            
        }
    
    }

    else if(tamanho_s1 == tamanho_s2)
        for(int i = 0; i < tamanho_s1; i++){
            if(((compara_caractere(*(s1+i), *(s2+i))))){
                cont++;
            }
                
        }

    if(cont == tamanho_s2){
        return 1;
    }

    return 0;
}

/*Função que apenas verifica se o nome tem barra, se tiver é porque é um diretorio */
int nome_barra(char *dest){
    for (int i = 0; dest[i] != '\0'; i++)
        if (dest[i] == '/')
            return 1;

    return 0;
}

/*Função que verifica se o arquivo existe na estrutura de diretorios, se existir retorna ele se não retorna NULL */
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

/*Função que recebe os conteudos de um arquivo, como nome, data, tamanho, etc. */
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

/*Função que sempre atualiza o conteudo de um arquivo */
struct nodoM* insereI(struct nodoM *nodo, char *nomeArq){
    nodo = conteudo(nodo, nomeArq);

    return nodo;
}

/*Função que atualiza o conteudo de um arquivo apenas se a data do atual for menor que a do anterior */
struct nodoM* insereA(struct nodoM *nodo, char *nomeArq){
    struct stat *compara;
    compara = malloc(sizeof(struct stat));
    stat(nomeArq, compara);

    if(compara->st_mtime < nodo->data)
        nodo = conteudo(nodo, nomeArq);

    return nodo;
}

/*Função que insere na estrutura diretorio os nodos e suas informações */
struct nodoM* insere(struct diretorio *d, char *nomeArq, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq)){
    struct nodoM *aux = NULL;
    
    if(diretorio_vazio(d)){
        if(!(d->inicio =  malloc(sizeof(struct nodoM))))
            return NULL;
        strcpy(d->inicio->nomeArq, nomeArq);

        d->inicio->tam_nome = strlen(nomeArq);
        d->inicio = conteudo(d->inicio, nomeArq);
        if(nome_barra(nomeArq) == 1){
            char caminho[TAM_NOME];
            caminho[0] = '.';
            caminho[1] = '\0';
            strcat(caminho, d->inicio->nomeArq);
            d->inicio->tam_nome++;
            strcpy(d->inicio->nomeArq, caminho);
        }
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
    if(nome_barra(nomeArq) == 1){
        char caminho[TAM_NOME];
        caminho[0] = '.';
        caminho[1] = '\0';
        strcat(caminho, aux->prox->nomeArq);
        aux->prox->tam_nome++;    
        strcpy(aux->prox->nomeArq, caminho);
    }
    aux->prox->ordem = aux->ordem + 1;
    aux->prox->localizacao = aux->localizacao + aux->tamanho;
    aux->prox->prox = NULL;

    d->inicio_diretorio = d->inicio_diretorio + aux->prox->tamanho;
    d->fim = aux->prox;

    return d->fim;
}

/*Função que insere o nodo de um arquivo logo após o target passado, se não existe é adicionado, se já existe é apenas movido */
int insere_diretorio_apos_target(struct diretorio *d, char *nomeArq, char *target){
    struct nodoM *aux, *novo, *temp, *temp2, *anterior;
    
    aux = existe_arq(d, target);

    if(aux == NULL)
        return 1;
    
    novo = existe_arq(d, nomeArq);

    if(novo == NULL)
        return 1;
    

    if(novo != NULL){
        
        if(aux->prox == novo)
            return 1;

        if(novo->ordem == d->fim->ordem){
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
                aux->prox->localizacao = aux->localizacao + aux->tamanho; 
                aux = aux->prox;
                i++;
            }
            aux->ordem = i;
            d->fim = aux;
            d->fim->prox = NULL;

            
            return 0;
        }

        if(novo->ordem == d->inicio->ordem){
            d->inicio = novo->prox;
            temp = aux->prox;
            anterior = d->inicio;
            if(aux->prox != NULL)
                aux->prox = novo;
            else{
                aux->prox = malloc(sizeof(struct nodoM));
                aux->prox = novo;
            }
            novo->prox = temp;

            int i = 0;
            d->inicio->localizacao = sizeof(long int);
            aux = d->inicio;
            while(aux->prox != NULL){
                aux->ordem = i;
                aux->prox->localizacao = aux->localizacao + aux->tamanho; 
                aux = aux->prox;
                i++;
            }

            aux->ordem = i;
            d->fim = aux;
            d->fim->prox = NULL;
        
            return 0;
        }
        
        if(novo->ordem > aux->ordem){
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

            aux = d->inicio;
            while(aux->prox != NULL){
                aux->prox->localizacao = aux->localizacao + aux->tamanho; 
                aux = aux->prox;
            }

            return 0;
        }

        if(novo->ordem < aux->ordem){
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
                aux->prox->localizacao = aux->localizacao + aux->tamanho;
                aux = aux->prox;
                i++;
            }

            aux->ordem = i;
            d->fim = aux;
            d->fim->prox = NULL;

            return 0;
        }  

    }

    
    return 0;
}

/*Função que remove um arquivo do diretório */
int remove_arquivo_diretorio(struct diretorio *d, char *nomeArc, char *nomeArq){
    int tam, ordem;
    struct nodoM *aux = NULL, *temp = NULL, *anterior = NULL;

    aux = existe_arq(d, nomeArq);
    if(aux == NULL)
        return 1;
    
    if(aux == d->inicio && aux == d->fim){
        d->inicio = NULL;
        d->inicio_diretorio = 0;
        return 0;  
    }

    else if(aux == d->inicio){
        d->inicio = d->inicio->prox;
        int tam = aux->tamanho;
        temp = d->inicio;

        for(int i = d->inicio->ordem; i <= d->fim->ordem; i++){
            temp->ordem = temp->ordem - 1;
            temp->localizacao = temp->localizacao - tam;
            
            temp = temp->prox;
        }

        d->inicio_diretorio = d->inicio_diretorio - tam;
        
        free(aux);
        return 0;
    }

    else if(aux == d->fim){
        anterior = d->inicio;
        tam = aux->tamanho;
        ordem = aux->ordem;

        while(anterior->prox != aux){
            anterior = anterior->prox;
        }
        
    
        d->fim = anterior;
        d->fim->prox = NULL;
        d->inicio_diretorio = d->inicio_diretorio - tam;

        free(aux);    
        return 0;
    }

    else{
        anterior = d->inicio;
        temp = aux->prox;

        while(anterior->prox != aux){
            anterior = anterior->prox;
        }
        anterior->prox = temp;
        tam = aux->tamanho;
        ordem = aux->ordem;
    
        

        for(int i = ordem; i <= d->fim->ordem; i++){
            temp->ordem = temp->ordem - 1;
            temp->localizacao = temp->localizacao - tam;
            
            temp = temp->prox;
        }

        d->inicio_diretorio = d->inicio_diretorio - tam;
        
        free(aux);
        return 0;
    }
    
    return 0;
}

/*Função auxiliar que diz se um diretorio está vazio */
int diretorio_vazio(struct diretorio *d){
    if(d->inicio == NULL)
        return 1;

    return 0;
}

/*Função auxiliar que imprime um diretorio com apenas informações consideradas importantes para depuração */
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

/*Função que libera a memória alocada pelo diretório */
void destroi_diretorio(struct diretorio *d){
    struct nodoM *aux = NULL;

    if(d == NULL)
        return;

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
