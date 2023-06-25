# Projeto4
Projeto 4 (vina++) da disciplina Algoritmos 3
Aluno: Matheus Sebastian Alencar de Carvalho. GRR20220065

Objetivo: é implementar o programa vina++1, que consiste de um arquivador básico (archiver), isto é, um programa que salva em sequência uma coleção de arquivos (denominados membros) dentro de outro arquivo (denominado archive) cuja estrutura permite recuperar os arquivos originais individualmente.

Foi enviado um diretorio com nome msac22, login no dinf, e dentro dele está o readme, o makefile e um diretório src apresenta os as bibliotecas e o vina++.c. Ao executar make all, o executável vina++ é gerado dentro do diretório msac22, e fora do diretório src. 

BUGS conhecidos:


    .Em alguns casos de comparações de nome, como do arquivo /home/matheusseb/Downloads/calgary/book1 e /home/matheusseb/Downloads/calgary/book2, a comparação estava considerando 1 e 2 como valores dseconhecidos e iguais, o que causa sobre escrita de um conteúdo no outro mas o nome continua o do primeiro;
    .Em alguns casos de move no meio do diretório, o conteúdo é "comido", o final e o começo aparentam estar corretos mas algo faz com que o conteúdo se corrompa no meio, e se isso acontecer a remoção também é prejudicada. (Um pouco mais de tempo e analise poderia resolver esse "bug");
    .Dependendo do editor de texto que é usado, um caractere adicional é colocado sempre ao final do mesmo arquivo, isso prejudicou um pouco a evolução do código por achar que o erro era meu e não do editor de texto que estava utilizando.

    
Para a implementação desse programa, foram utilizadas duas bibliotecas, uma focada na lista de diretórios e a outra na manipulação do archive.
Na "libDiretorio.h", foi utilizada uma lista ligada simples, com um ponteiro para o incio, um para o fim e um long int que representa o valor de inicio do diretorio no Archive. Cada nodo dessa lista apresenta uma estrutura específica, sendo ela:


    Nome do arquivo, com no máximo 63 caracteres: char nomeArq[TAM_NOME];
    Tamanho do nome do arquivo: int tam_nome;
    Id do usuário que o arquivo pertence: uid_t uid;
    Permissões do arquivo: mode_t permissoes;
    Tamanho em bytes do arquivo: off_t tamanho;
    Data de criação ou última atualização do arquivo: _time_t data;
    Ponteiro para o próximo nodo: struct nodoM *prox; 
    Ordem do arquivo dentro da lista: int ordem;
    Localização do inicio do conteudo do diretorio no Archive: long int localizacao;

Para a obtenção dos valores dos arquivos, foi utilizada a biblioteca stat.h, a qual consegue extrair os dados necessários para construção de um dado dentro da lista de diretório, como data, userID, permissões, etc. Alguns valores que essa função retorna não são utilizados por conta das especificações desse trabalho.
Todas as manipulações dentro da estrutura diretório são simples, já que as manipulações são feitas seguindo a ideia de uma fila, na qual a cada novo arquivo inserido na lista sua posição será a última, com o ponteiro para o final sendo atualizado para ele, e aumentando o valor do inicio do diretório de acordo com o novo tamanho. 


        A opção -i está associada a função insereI, que atualiza o nodo se ele já existir e atualiza suas propriedades. Já a opção -a atualiza apenas se o conteudo a ser inserido atualmente é mais recente que o anterior, sendo representado pela função insereA. 
        A opção -m, algum nodo já existente ou não é inserido logo após outro nodo já existente na lista, e como a lista implementada não é duplamente ligada, algumas operações são um pouco mais custosas que se fosse duplamente ligada, já que é necessario salvar os nodos para não perder pedaços da lista.
        A opção -x gera um arquivo de saída para um arquivo em específico ou para todos na lista, utilizando apenas do nome do arquivo e o conteudo dele, já que não foi especificado a necessidade de outras atribuições para essa extração, apenas nome e conteúdo idênticos. 
        A opção -r apenas remove o nodo da lista de diretórios e atualiza a ordem e o inicio do diretorio no archive. Essa remoção também seria facilitada caso a lista fosse duplamente ligada, mas como a quantidade de inserções pode ser muito grande, foi escolhido a utilização de uma lista simplesmente ligada para utilizar menos memória, já que seria necessário um ponteiro a mais para cada nodo.
        A opção -c utiliza fundamentalmente da lista de diretórios, mas foi colocada na biblioteca "libArchive.h" pois o diretorio em questão é recebido a partir de um archive já existente. 
        A opção -h é apenas orientadora, a qual não utiliza nada da biblioteca libDiretorio.h

Algumas funções auxiliares da libDiretorio devem ser destacadas:

    
    Existe_arq(struct diretorio *d, char *nomeArq): verifica se algum nodo apresenta mesmo nome que o nome passado, se o nome no diretorio apresenta / ele começa com ./, logo temos que comparar a partir do próximo caractere, já que não é passado ./caminho;
    Diretorio_vazio(struct diretorio *d): verifica se o diretório está vazio;
    Destroi_diretorio(struct diretorio *d): Libera memória alocada para a estrutura;
    Imprime_diretorio(struct diretorio *d): Função para depuração própria, não utilizada diretamente no programa.

A "libArchive.h" apresenta funções focadas na manipulação dos arquivos binários em si, como a abertura para leitura e escrita e apenas leitura. Algumas mensaegns de erros são apresentadas nessa função quando o arquivo não pode ser aberto ou não existe, se o target ou o valor que será movimentado não existe no archive também imprime uma mensaegm de erro e interrompe imediatamente o programa.  

    
        .Opção -i: Toda inserção passa pela função "atualiza_conteudo", em que se o arquivo já existir no diretório do archive correspondente o conteúdo é atualizado se existir ou inserido caso não exista. A manipulação do movimento do conteúdo não foi feito utilizando uma função específica pois os parâmetros que teriam que ser passados para analisar o inicio, quantidade de blocos e destino seria trabalhosa de analisar, então foi feita manualmente repetindo os blocos de manipulações de conteúdo, que são sempre lidos em no máximo BUFFER_SIZE (1024 bytes).
