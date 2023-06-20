# Projeto4
Projeto 4 (vina++) da disciplina Algoritmos 3
Aluno: Matheus Sebastian Alencar de Carvalho. GRR20220065

Objetivo: é implementar o programa vina++1, que consiste de um arquivador básico (archiver), isto é, um programa que salva em sequência uma coleção de arquivos (denominados membros) dentro de outro arquivo (denominado archive) cuja estrutura permite recuperar os arquivos originais individualmente.

Para a implementação desse programa, foi utilizada uma lista ligada simples, com um ponteiro para o incio, um para o fim e um long int que representa o valor de inicio do diretorio no Archive. Cada nodo dessa lista apresenta uma estrutura específica:


    Nome do arquivo, com no máximo 63 caracteres: char nomeArq[TAM_NOME];
    Tamanho do nome do arquivo: int tam_nome;
    Id do usuário que o arquivo pertence: uid_t uid;
    Permissões do arquivo: mode_t permissoes;
    Tamanho em bytes do arquivo: off_t tamanho;
    Data de criação ou última atualização do arquivo: _time_t data;
    Ponteiro para o próximo nodo: struct nodoM *prox; 
    Ordem do arquivo dentro da lista: int ordem;
    Localização do inicio do conteudo do diretorio no Archive: long int localizacao;
