void atualiza_conteudo(struct diretorio *d, char *nomeArc, char *nomeArq){
    char buffer[BUFFER_SIZE] = {0};
    FILE *archive, *destino;
    struct nodoM *aux = NULL;
    long int tam_ant_arq = 0, diferenca = 0, fim = 0;
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
    fseek(archive, aux->localizacao + tam_ant_arq, SEEK_SET);

    if(diferenca > 0){
        if(aux->prox != NULL){
            int blocos = aux->prox->localizacao / BUFFER_SIZE;
            int resto = aux->prox->localizacao % BUFFER_SIZE;
            fseek(archive, b->fim->localizacao, SEEK_SET);
            
        }
    }

    
    for(int i = aux->ordem; i <= b->fim->ordem - 1; i++){
        printf("Localização antiga do prox arq: %ld\n", aux->prox->localizacao);
        aux->prox->localizacao = aux->prox->localizacao + diferenca;
        printf("Localização do prox arq: %ld\n", aux->prox->localizacao);
            
        aux = aux->prox;
    }
    
    destino = abre_archive_leitura_escrita(nomeArq);
    


    return;
}
