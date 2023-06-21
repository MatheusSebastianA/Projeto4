#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pwd.h>
#include "libArchive.h"

#define _XOPEN_SOURCE_EXTENDED 1
#define BUFFER_SIZE 1024

/*Função que abre um arquivo para leitura e escrita binaria, se ele não existe é aberto com apenas wb para que ele seja criado vazio */
FILE* abre_archive_leitura_escrita(char *nomeArq){
    FILE *arq;
    arq = fopen(nomeArq, "r+b");

    if(arq == NULL)
        arq = fopen(nomeArq, "wb");

    if(arq == NULL)
        return NULL;
    
    return arq;
}

/*Função que abre um arquivo apenas para leitura, se não existir retorna NULL */
FILE* abre_archive_leitura(char *nomeArq){
    FILE *arq;
    arq = fopen(nomeArq, "r");

    if(arq == NULL)
        return NULL;
    
    return arq;
}

/*Função que extrai os dados de um archive e insere na lista de diretorios */
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
        fread(aux->nomeArq, sizeof(char), aux->tam_nome, arc);
        fread(&aux->data, sizeof(time_t), 1, arc);
        fread(&aux->localizacao, sizeof(long int), 1, arc);
        fread(&aux->ordem, sizeof(int), 1, arc);
        fread(&aux->permissoes, sizeof(mode_t), 1, arc);
        fread(&aux->tamanho, sizeof(off_t), 1, arc);
        fread(&aux->uid, sizeof(uid_t), 1, arc);

        if(diretorio_vazio(d)){
            d->inicio = aux;
            d->inicio->prox = NULL;
            d->inicio_diretorio = ini_dir;
            d->fim = d->inicio;
            d->fim->prox = NULL;
        }

        else{
            d->fim->prox = aux;
            d->fim = aux;
            d->fim->prox = NULL;
            d->inicio_diretorio = ini_dir;
        }


        aux = aux->prox;
    }

    fclose(arc);

    return d;
}

/*Função que insere os dados da lista de diretorio em um archive pela sua ordem */
void insere_diretorio(struct diretorio *d, char *nomeArc){
    struct nodoM *aux = NULL;
    FILE *arc = abre_archive_leitura_escrita(nomeArc);   

    if(!arc)
        return;
        
    aux = d->inicio;
    fseek(arc, d->inicio_diretorio, SEEK_SET);

    for(int i = 0; i <= d->fim->ordem; i++){
        fwrite(&aux->tam_nome, sizeof(int), 1, arc);
        fwrite(aux->nomeArq, sizeof(char), aux->tam_nome, arc);
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

/*Função que extrai o conteudo de um arquivo com menos de 1024 bytes e insere no archive na posição atual do archive */
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

/*Função que extrai o conteudo de um arquivo com tamanho de 1024 bytes e insere no archive na posição atual do archive */
int insere_bloco_conteudo(struct nodoM *nodo, FILE *arq, FILE *archive){
    char buffer[BUFFER_SIZE] = {0};
    for(int i = 0; i < 1024; i++){
        fread(&buffer[i], sizeof(char), 1, arq);
        fwrite(&buffer[i], sizeof(char), 1, archive);
    }
    

    return 0;
}

/*Função que insere conteudo na lista de diretorio e no archive */
int insere_conteudo(struct diretorio *d, char *nomeArq, char *nomeArc, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq)){
    char buffer[BUFFER_SIZE] = {0};
    int blocos = 0, resto = 0, temp;
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
        temp = blocos;
        if(blocos >= 1)
            for(blocos = blocos; blocos > 0; blocos--)
                insere_bloco_conteudo(d->inicio, arq, arc);

        fseek(arq, 1024*temp, SEEK_SET);
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
    temp = blocos;
    if(blocos >= 1)
        for(blocos = blocos; blocos > 0; blocos--){
            fseek(arq, 0, SEEK_SET);
            fread(buffer, sizeof(char), BUFFER_SIZE, arq);
            fseek(arc, d->inicio_diretorio - aux->tamanho + (temp - blocos)*1024, SEEK_SET);
            fwrite(buffer, sizeof(char), BUFFER_SIZE, arc); 
        }  

    fseek(arq, 1024*temp, SEEK_SET);
    if(blocos < 1 && resto != 0)
        insere_conteudo_menor1024(aux, arq, arc);
    
    
    fclose(arq);
    fclose(arc);
    
    return 0;
}

/*Função que insere conteudo na lista de diretorio e no archive apos target*/
void insere_conteudo_apos_target(struct diretorio *d, char *nomeArq, char *target,  char *nomeArc, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq)){
    char buffer[BUFFER_SIZE] = {0};
    struct nodoM *aux, *novo, *temp;
    int inicio, blocos, resto, i, cont;
    FILE *archive = NULL;

    archive = abre_archive_leitura_escrita(nomeArc);
    if(archive == NULL)
        return;

    aux = existe_arq(d, target);
    if(aux == NULL)
        return;
    
    novo = existe_arq(d, nomeArq);

    if(novo != NULL){
        if(aux->prox == novo)
            return;

        if(novo->ordem == d->fim->ordem){
            inicio = aux->prox->localizacao;
            blocos = (d->inicio_diretorio - inicio) / BUFFER_SIZE;
            resto = (d->inicio_diretorio - inicio) % BUFFER_SIZE;
                
            cont = blocos + 1;
            if(blocos >= 1)
                for(blocos = blocos; blocos > 0; blocos--){
                    fseek(archive, -(cont-blocos)*1024, SEEK_END);
                    fread(buffer, sizeof(char), 1024, archive);
                    fseek(archive, d->inicio_diretorio + resto, SEEK_SET);
                    fwrite(buffer, sizeof(char), 1024, archive);
                }
    
            if(blocos < 1 && resto != 0){
                fseek(archive, inicio, SEEK_SET);
                for(i = 0; i < resto; i++){
                    fread(&buffer[i], sizeof(char), 1, archive);
                }
                fseek(archive, d->inicio_diretorio, SEEK_SET);
                fwrite(&buffer, sizeof(char), resto, archive);
            }
            fseek(archive, 0, SEEK_END);
            truncate(nomeArc, ftell(archive) - (novo->localizacao - aux->prox->localizacao));
            fseek(archive, 0, SEEK_END);
            
            blocos = cont - 1;
            cont = blocos;
            if(blocos >= 1)
                for(blocos = blocos; blocos > 0; blocos--){
                    fseek(archive, novo->localizacao + (cont - blocos)*1024, SEEK_SET);
                    fread(buffer, sizeof(char), BUFFER_SIZE, archive);
                    fseek(archive, inicio, SEEK_SET);
                    fwrite(buffer, sizeof(char), BUFFER_SIZE, archive);
                }

            if(blocos < 1 && resto != 0){
                fseek(archive, d->inicio_diretorio + (cont*1024) - novo->tamanho, SEEK_SET);
                for(i = 0; i < resto; i++){
                    fread(&buffer[i], sizeof(char), 1, archive);
                }
                fseek(archive, inicio + cont*1024, SEEK_SET);
                fwrite(&buffer, sizeof(char), resto, archive);
            }
            truncate(nomeArc, d->inicio_diretorio);
            
            insere_diretorio_apos_target(d, nomeArq, target);
            temp = d->inicio;
            for(int i = temp->ordem; i < d->fim->ordem; i++){
                temp->prox->localizacao = temp->localizacao + temp->tamanho; 
                temp = temp->prox;
            }  
             
            return;
        }

        if(novo->ordem == d->inicio->ordem){
            printf("AQUI VEY 2\n");
            if(aux != d->fim)
                inicio = aux->prox->localizacao;
            else{
                inicio = d->inicio_diretorio;
                truncate(nomeArc, inicio);
            }

            blocos = (d->inicio_diretorio - inicio) / BUFFER_SIZE;
            resto = (d->inicio_diretorio - inicio) % BUFFER_SIZE;
            cont = blocos + 1;
            printf("RESTO:%d\n", resto);
            if(blocos >= 1)
                for(blocos = blocos; blocos > 0; blocos--){
                    fseek(archive, -((cont - blocos)*1024), SEEK_END);
                    fread(buffer, sizeof(char), BUFFER_SIZE, archive);
                    fseek(archive, -1024*blocos + novo->tamanho, SEEK_CUR);
                    fwrite(buffer, sizeof(char), BUFFER_SIZE, archive);
                }

            if(blocos < 1 && resto != 0){
                fseek(archive, inicio, SEEK_SET);
                for(i = 0; i < resto; i++){
                    fread(&buffer[i], sizeof(char), 1, archive);
                }
                fseek(archive, inicio + novo->tamanho, SEEK_SET);
                fwrite(&buffer, sizeof(char), resto, archive);
            }

            blocos = (novo->tamanho) / BUFFER_SIZE;
            resto = (novo->tamanho) % BUFFER_SIZE;
            printf("RESTO:%d\n", resto);
            cont = blocos;
            if(blocos >= 1)
                for(blocos = blocos; blocos > 0; blocos--){
                    fseek(archive, novo->localizacao + resto + (blocos - 1)*1024, SEEK_SET);
                    fread(buffer, sizeof(char), BUFFER_SIZE, archive);
                    fseek(archive, aux->localizacao + aux->tamanho + resto , SEEK_SET);
                    fwrite(buffer, sizeof(char), BUFFER_SIZE, archive);
                }

            if(blocos < 1 && resto != 0){
                fseek(archive, novo->localizacao, SEEK_SET);
                for(i = 0; i < resto; i++){
                    fread(&buffer[i], sizeof(char), 1, archive);
                }
                fseek(archive, aux->localizacao + aux->tamanho, SEEK_SET);
                fwrite(&buffer, sizeof(char), resto, archive);
            }
            
            fseek(archive, 0, SEEK_END);
            blocos = (ftell(archive) - (novo->localizacao + novo->tamanho)) / BUFFER_SIZE;
            resto = (ftell(archive) - (novo->localizacao + novo->tamanho)) % BUFFER_SIZE;
            cont = blocos;
            if(blocos >= 1)
                for(blocos = blocos; blocos > 0; blocos--){
                    fseek(archive, d->inicio->prox->localizacao + (cont - blocos)*1024, SEEK_SET);
                    fread(buffer, sizeof(char), BUFFER_SIZE, archive);
                    fseek(archive, d->inicio->localizacao + (cont - blocos)*1024, SEEK_SET);
                    fwrite(buffer, sizeof(char), BUFFER_SIZE, archive);
                }

            if(blocos < 1 && resto != 0){
                fseek(archive, d->inicio->prox->localizacao + (cont*1024), SEEK_SET);
                for(i = 0; i < resto; i++){
                    fread(&buffer[i], sizeof(char), 1, archive);
                }
                fseek(archive, d->inicio->localizacao + cont*1024, SEEK_SET);
                fwrite(&buffer, sizeof(char), resto, archive);
            } 
            truncate(nomeArc, d->inicio_diretorio);
            temp = d->inicio->prox;
            insere_diretorio_apos_target(d, nomeArq, target);
            d->inicio = temp;
            temp->localizacao = sizeof(long int);
            for(int j = 0; j < d->fim->ordem - 1; j++){
                temp->prox->localizacao = temp->localizacao + temp->tamanho;
                temp = temp->prox;
            }
            
            return;

        }






    }

    return;
}

/*Função que apenas extrai o conteudo de um arquivo e insere no arquivo de nome correspondete */
void extrai_conteudo_arquivo(char *arc, char *dest){
    FILE *archive, *destino;
    struct nodoM *aux = NULL;
    struct diretorio *b = cria_diretorio();
    recebe_diretorio(b, arc);

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

/*Função que remove o conteudo de um arquivo no archive e na lista de diretorios. */
void remove_conteudo(struct diretorio *d, char *arc, char *arq){
    char buffer[BUFFER_SIZE] = {0};
    int inicio, tam, blocos, resto, i, cont;
    struct nodoM *aux = NULL, *temp = NULL, *anterior = NULL;
    FILE* archive = NULL;

    archive = abre_archive_leitura_escrita(arc);
    if(archive == NULL){
        printf("Erro ao abrir %s\n", arc);
        return;
    }

    aux = existe_arq(d, arq);
    if(aux == NULL){
        printf("O arquivo %s não está no Archive\n", arc);
        return;
    }
        
    truncate(arc, d->inicio_diretorio);

    if(aux == d->inicio && aux == d->fim){
        destroi_diretorio(d);
        truncate(arc, 0);
        return;
    }

    else if(aux == d->inicio){
        inicio = aux->prox->localizacao;
        tam = d->inicio_diretorio - aux->prox->localizacao;

        fseek(archive, d->inicio->localizacao, SEEK_SET);
        blocos = tam / BUFFER_SIZE;
        resto = tam % BUFFER_SIZE;
        cont = blocos;
        if(blocos >= 1)
            for(blocos = blocos; blocos > 0; blocos--){
                fseek(archive, inicio + ((cont - blocos)*1024), SEEK_SET);
                fread(buffer, sizeof(char), BUFFER_SIZE, archive);
                fseek(archive, sizeof(long int) + ((cont - blocos)*1024), SEEK_SET);
                fwrite(buffer, sizeof(char), BUFFER_SIZE, archive);
            } 

        if(blocos < 1 && resto != 0){
            fseek(archive, aux->prox->localizacao + cont*1024, SEEK_SET);
            for(i = 0; i < resto; i++){
                fread(&buffer[i], sizeof(char), 1, archive);
            }
            fseek(archive, aux->localizacao + cont*1024, SEEK_SET);
            fwrite(&buffer, sizeof(char), resto, archive);
        } 

        truncate(arc, d->inicio_diretorio - aux->tamanho);
        remove_arquivo_diretorio(d, arc, arq);

        return;
    }

    else if(aux == d->fim){
        
        return;
    }

    else{
        
        return;
    }
    
    return;
}

/*Função que atualiza o conteudo de um arquivo no archive e na lista de diretorios, sua posicao permanece a mesma */
void atualiza_conteudo(struct diretorio *d, char *nomeArq, char *nomeArc, struct nodoM* (* func) (struct nodoM *aux, char *nomeArq)){
    char buffer[BUFFER_SIZE] = {0};
    FILE *archive, *arquivo;
    struct nodoM *aux = NULL;
    int i, blocos, resto;
    long int tam_ant_arq = 0, diferenca = 0;

    if(diretorio_vazio(d)){
        insere_conteudo(d, nomeArq, nomeArc, func);
        return;
    }

    aux = existe_arq(d, nomeArq);
    if(aux == NULL){
        insere_conteudo(d, nomeArq, nomeArc, func);
        return;
    }

    tam_ant_arq = aux->tamanho;
    conteudo(aux, nomeArq);
    diferenca = aux->tamanho - tam_ant_arq;

    archive = abre_archive_leitura_escrita(nomeArc);
    if(archive == NULL)
        return;
    
    fseek(archive, 0, SEEK_END);
    rewind(archive);
    fread(&d->inicio_diretorio, sizeof(long int), 1, archive);
    fseek(archive, aux->localizacao + tam_ant_arq, SEEK_SET);

    if(diferenca > 0){
        blocos = (d->inicio_diretorio - ftell(archive)) / BUFFER_SIZE;
        resto = (d->inicio_diretorio - ftell(archive)) % BUFFER_SIZE;
        i = blocos;
        if(blocos >= 1)
            for(blocos = blocos; blocos > 0; blocos--){
                fseek(archive, aux->localizacao + tam_ant_arq + (i - blocos)*1024, SEEK_SET);
                fread(buffer, sizeof(char), BUFFER_SIZE, archive);
                fseek(archive, diferenca, SEEK_CUR);
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

    else{
        blocos = (d->inicio_diretorio - ftell(archive)) / BUFFER_SIZE;
        resto = (d->inicio_diretorio - ftell(archive)) % BUFFER_SIZE;
        i = blocos;
        if(blocos >= 1)
            for(blocos = blocos; blocos > 0; blocos--){
                fseek(archive, aux->localizacao + tam_ant_arq + (i - blocos)*1024, SEEK_SET);
                fread(buffer, sizeof(char), BUFFER_SIZE, archive);
                fseek(archive, diferenca - (i - blocos)*1024, SEEK_CUR);
                fwrite(buffer, sizeof(char), BUFFER_SIZE, archive);
            }
        if(blocos < 1 && resto != 0){
            for(i = 0; i < resto; i++){
                fread(&buffer[i], sizeof(char), 1, archive);
            }
            
            fseek(archive, aux->localizacao + aux->tamanho + (blocos * 1024), SEEK_SET);
            fwrite(&buffer, sizeof(char), resto, archive);
        }
        truncate(nomeArc, aux->localizacao + aux->tamanho);
    }
    arquivo = abre_archive_leitura(nomeArq);

    if(!arquivo)
        return;

    blocos = aux->tamanho / BUFFER_SIZE;
    resto = aux->tamanho % BUFFER_SIZE;
    fseek(archive, aux->localizacao, SEEK_SET);
    if(blocos >= 1)
        for(blocos = blocos; blocos > 0; blocos--)
            insere_bloco_conteudo(aux, arquivo, archive);

    if(blocos < 1 && resto != 0)
        insere_conteudo_menor1024(aux, arquivo, archive);

    
    for(int i = aux->ordem; i <= d->fim->ordem - 1; i++){
        aux->prox->localizacao = aux->prox->localizacao + diferenca;
            
        aux = aux->prox;
    }
    d->inicio_diretorio = d->inicio_diretorio + diferenca;
    rewind(archive);
    fwrite(&d->inicio_diretorio, sizeof(long int), 1, archive);
    insere_diretorio(d, nomeArc);

    fclose(archive);
    fclose(arquivo);

    return;
}

/*Função que apenas imprime as permissoes de um arquivo */
void imprime_permissoes(mode_t mode){
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");

    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");

    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

/*Função que apenas imprime todas as informacoes de um arquivo */
void imprime_informacoes(struct diretorio *d, char *nomeArc){
    struct tm *time;
    struct nodoM *aux;
    struct passwd *pw;
    FILE *arc;
    arc = abre_archive_leitura_escrita(nomeArc);
    if(arc == NULL)
        return;

    aux = d->inicio;

    for(int i = d->inicio->ordem; i <= d->fim->ordem; i++){
        pw = getpwuid(aux->uid);
        time = localtime(&aux->data);
        imprime_permissoes(aux->permissoes);
        printf(" %s", pw->pw_name);
        printf("%8ld ", aux->tamanho);
        printf("%d-%02d-%02d %02d:%02d ", time->tm_year+1900, time->tm_mon+1, time->tm_mday, time->tm_hour, time->tm_min);
        printf("%s\n", aux->nomeArq);
        
        aux = aux->prox;
    }

    fclose(arc);
    return;
}

void imprime_opcoes(){
    printf("Opções disponíveis:\n");
    printf("./vina++ -i  <archive> [membro1 membro2 ...]\n");
    printf("./vina++ -a  <archive> [membro1 membro2 ...]\n");
    printf("./vina++ -m  [target] <archive> [membroX]\n");
    printf("./vina++ -x  [membro1 membro2 ...] | vina++ -x\n");
    printf("./vina++ -r  <archive> [membro1 membro2 ...]\n");
    printf("./vina++ -c  <archive>\n");
    printf("./vina++ -h\n");

    return;
}
