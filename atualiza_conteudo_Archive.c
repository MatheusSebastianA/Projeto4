void atualiza_conteudo(struct diretorio *d, char *nomeArq, char *nomeArc){
    char buffer[BUFFER_SIZE] = {0};
    FILE *archive, *arquivo;
    struct nodoM *aux = NULL;
    int i = 0, blocos, resto;
    long int tam_ant_arq = 0, diferenca = 0, fim = 0, teste = 0;
    struct diretorio *b = cria_diretorio();
    recebe_diretorio(b, "backup.txt", insereI);
    imprime_diretorio(b);

    aux = existe_arq(b, nomeArq);
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
    fread(&b->inicio_diretorio, sizeof(long int), 1, archive);
    fseek(archive, aux->localizacao + tam_ant_arq, SEEK_SET);

    if(diferenca > 0){
        teste = ftell(archive);
        blocos = (b->inicio_diretorio - ftell(archive)) / BUFFER_SIZE;
        resto = (b->inicio_diretorio - ftell(archive)) % BUFFER_SIZE;
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
    if(blocos >= 1)
        for(blocos = blocos; blocos > 0; blocos--){
            fread(buffer, sizeof(char), BUFFER_SIZE, arquivo);
            fseek(archive, aux->localizacao, SEEK_CUR);
            fwrite(buffer, sizeof(char), BUFFER_SIZE, archive);
        }
    if(blocos < 1 && resto != 0){
        for(int i = 0; i < resto; i++)
            fread(&buffer[i], sizeof(char), 1, arquivo);

        fseek(archive, aux->localizacao + (blocos * 1024), SEEK_SET);
        fwrite(&buffer, sizeof(char), resto, archive);
    }    

    
    for(int i = aux->ordem; i <= b->fim->ordem - 1; i++){
        aux->prox->localizacao = aux->prox->localizacao + diferenca;
            
        aux = aux->prox;
    }
    printf("Depois das att:\n");
    imprime_diretorio(b);
    


    return;
}
