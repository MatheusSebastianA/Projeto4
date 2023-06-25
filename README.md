# Projeto4
Projeto 4 (vina++) da disciplina Algoritmos 3
Aluno: Matheus Sebastian Alencar de Carvalho. GRR20220065

Objetivo: é implementar o programa vina++1, que consiste de um arquivador básico (archiver), isto é, um programa que salva em sequência uma coleção de arquivos (denominados membros) dentro de outro arquivo (denominado archive) cuja estrutura permite recuperar os arquivos originais individualmente.

Foi enviado um diretorio com nome msac22, login no dinf, e dentro dele está o readme, o makefile e um diretório src apresenta os as bibliotecas e o vina++.c. Ao executar make all, o executável vina++ é gerado dentro do diretório msac22, e fora do diretório src. 

BUGS conhecidos:


    Em alguns casos de comparações de nome, como do arquivo /home/matheusseb/Downloads/calgary/book1 e /home/matheusseb/Downloads/calgary/book2, a comparação estava considerando 1 e 2 como valores dseconhecidos e iguais, o que causa sobre escrita de um conteúdo no outro mas o nome continua o do primeiro;
    Em alguns casos de move no meio do diretório, o conteúdo é "comido", o final e o começo aparentam estar corretos mas algo faz com que o conteúdo se corrompa no meio, e se isso acontecer a remoção também é prejudicada. (Um pouco mais de tempo e analise poderia resolver esse "bug");
    Dependendo do editor de texto que é usado, um caractere adicional é colocado sempre ao final do mesmo arquivo, isso prejudicou um pouco a evolução do código por achar que o erro era meu e não do editor de texto que estava utilizando;
    Erros podem causar porblemas de vazamento de memória;
    O programa considera que será passado apenas o caminho absoluto, ou seja, /home/inf/xyz00/texto.doc, e não ./home/inf/xyz00/texto.doc;
    Não é um bug, mas a grande repetição do bloco de movimento de blocos foi escolhida porque passar diversos parâmetros estava mais confundindo do que aujdando, e como alguns casos de movimentação eram específicos, foi escolhido repití-los, mesmo não sendo a melhor escolhe visualmente falando. 

    
Para a implementação desse programa, foram utilizadas duas bibliotecas, uma focada na lista de diretórios e a outra na manipulação do archive.
Na "libDiretorio.h", foi utilizada uma lista ligada simples, com um ponteiro para o incio, um para o fim e um long int que representa o valor de inicio do diretorio no Archive. Cada nodo dessa lista apresenta uma estrutura específica, sendo ela:


    Nome do arquivo, com no máximo 127 caracteres: char nomeArq[TAM_NOME];
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
        A opção -m: Algum nodo já existente ou não é inserido logo após outro nodo já existente na lista, e como a lista implementada não é duplamente ligada, algumas operações são um pouco mais custosas que se fosse duplamente ligada, já que é necessario salvar os nodos para não perder pedaços da lista.
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

    
        Opção -i: Toda inserção passa pela função "atualiza_conteudo", em que se o arquivo já existir no diretório do archive correspondente o conteúdo é atualizado se existir ou inserido caso não exista. A manipulação do movimento do conteúdo não foi feito utilizando uma função específica pois os parâmetros que teriam que ser passados para analisar o inicio, quantidade de blocos e destino seria trabalhosa de analisar, então foi feita manualmente repetindo os blocos de manipulações de conteúdo, que são sempre lidos em no máximo BUFFER_SIZE (1024 bytes).
        Opção -a: Funciona da mesma forma que a opção -a, mas só vai atualizar o conteúdo se a data do arquivo for menor que a antiga data. Se o arquivo não existe, ele é inserido.
        Opção -m: A opção de move foi sem dúvidas a maior dificuldade nesse trabalho, em que, além de atualizar a lista de diretórios sobre as respectivias mudanças, as mudanças no conteúdo são trabalhosas, tendo em vista que é necessário puxar uma certa quantidade de conteúdo dependendo do move que está sendo feito, e isso essa movimentação é feita em no máximo blocos de 1024, o que siginifica que se for inserir um conteúdo alguma posições para frente, é necessário lê-lo de trás para frente, para não haver perdas de conteúdo e se for necessário escrevê-lo algumas posições para trás, é necessário lê-lo do começo para o final. A grande dificuldade foi analisar da onde teria que começar a leitura e para onde ela deveria ir, alguns casos testados aparentam estar corretos, mas bem provavel que algum move entre valores do meio pode causar erros dependendo do tamanho dos dois conteúdos.
        Opção -x: A opção de extração analisa os conteúdos da parte do diretório para a criação dos diretórios necessários, se o nome de um arquivo for ./home/inf/xyz00/texto.doc, será criado, no diretório msac22, o diretório home, dentro do home será criado o diretório inf, dentro do diretório inf será cirado o diretório xyz00, e dentro desse último diretório será criado o arquivo texto.doc com o conteúdo referentes a ele dentro do archive.
        Opção -r: Assim como no move, algumas manipulações de conteúdo tem que ser feitas, mas são menos casos de análise, já que a remoção pode ser apenas no meio, no início ou final do archive. Se for no início, é necessário puxar todo o conteúdo na frente dele para o começo do archive. Se for no final, apenas trunca o archive e depois insere o diretório naquele local. Se for no meio, é necessário analisar os conteúdos que vêm depois do arquivo a ser removido e puxar eles para onde era o início do arquivo removido.
        Opção -c: Lê o long int que está no começo do arquivo, com esse valor podemos acessar a posição em que o diretório se inicia e a partir dela ir lendo e inserindo em um diretório, para posteriormente lê-lo segundo o formato tar com as opções tvf.
         Opção -h: Função apenas orientadora, a qual utiliza uma função da biblioteca libArchive.h com apenas printf.

Algumas dificuldades encontradas ao decorrer da implementação foram:
        
        Nome de um arquivo: Estava tendo problemas na hora da leitura do nome de um arquivo na estrutura diretório, pois estava colocando o nome no inicio do nodo e era necessário saber o tamanho para a leitura. O problema foi resolvido poruqe ao invés de utilizar um ponteiro para char foi escolhido utilizar um vetor com no máximo 127 caracteres, que é um tamanho aceitável para um arquivo e um caminho absoluto até ele e um inteiro que salva o tamanho do nome desse arquivo para posterior leitura denrto do archive.
        Movimentação do conteúdo em blocos: O primeiro problema enfretado foi que a utilização da função insere_bloco_conteúdo estava sendo confusa com relação aos parâmetros de inicio de leitura e escrita. A resolução foi inserir, repetidamente, o bloco que faz essa manipulação de acordo com a necessidade atual, o que não é uma boa prática, mas pela prioridade em fazer uma manipulação efetiva era mais fácil testar cada bloco separadamente com condições específicas.
        Arquivos passados com caminho absoluto: A dificuldade foi ter que verificar, antes de inserir no diretório, se o nome em questão apresenta alguma barra, pois se apresentar significa que é um caminho para diretorio, logo é necessário fazer uma concatenação com outra string iniciada com "." na primeira posição.
        Comparação de nomes: Alguns bugs aconteceram e prejudicaram a evolução do trabalho, a comparação dos nomes /home/matheusseb/Downloads/calgary/book1 e /home/matheusseb/Downloads/calgary/book2 era considerada igual, uma vez que os caracteres "1" e "2" não estavam sendo comparados corretamente, não consegui resolver esse problema pela falta de entendimento sobre a causa.
        Criação de diretórios: Estava tentando salvar em um vetor de strings os nomes dos diretórios, indo do pai até o menor filho, só que esse armazenamento estava, por algum motivo desconhecido, escrevendo apenas o último nome. A resolução foi, a partir do caminho ./home/matheusseb/Downloads/calgary/book1 pegar todo nome depois da barra, se o último caractere não for igual ao tamanho, significa que esse nome representa um diretório, se for a última letra, entrão esse nome representa o conteúdo que será criado. A criação de diretórios é feita concatenando a partir do primeiro nome encontrado até o final. 
        Move de conteúdo: A maior dificuldade do trabalho foi fazer essa opção. As dificuldades são baseadas na grande quantidade de possibilidade diferentes de movimentação. Se movermos um arquivo do inicio, é necessário puxar todo conteúdo para trás depois. Se movermos um arquivo do final, o arquivo do final é alterado e é necessário puxar todo o conteúdo que fica depois do target para n posições para frente, depois puxá-las para depois do arquivo modificado. Se movermos um arquivo que está antes do target, é necessário abrir espaço para ele, colocá-lo no lugar certo e depois puxar tudo para onde era sua posição de inicio. Se movermos um arquivo que está depois do target, é necessário puxar todo o conteúdo depois do target para frente, colocar o conteúdo no local, puxar tudo que estava depois do arquivo modificado para onde era sua posição antiga. Sem dúvidas essa foi a parte mais trabalhosa do trabalho e pode apresentar alguns bugs, como perde de conteúdo.

        
