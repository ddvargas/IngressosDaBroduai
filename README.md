# IngressosDaBroduai

Programa criado como parte da requisição para aprovação na disciplina de Sistemas Operacionais do curso de Sistemas de Informação da Universidade Federal de Santa Maria. O conteúdo principal é que se use threads e concorrências.

O intuito é que se fornceça um arquivo de configuração, contendo dados de eventos. O programa lê esses dados e simula a interação de vários clientes tentando comprar ingressos nesses eventos por meio do uso de threads e concorrência. Um sistema de recomendação é usado para que threads possam trocar de eventos, caso não haja mais lugares disponíveis no evento original delas. Durante a execução um trace do que está acontecendo é printado na saída padrão. Ao final da execução é apresentado um resultado com os dados do evento e um trace dos eventos do programa.

## Modo e uso
Criar um arquivo de entrada contendo os dados dos eventos (nome|NumeroLugares|ValorIngresso|NumeroThreads) todos separados por um pipe, um evento por linha. O arquivo deve ser nomeado como "input.txt" e colocado no mesmo diretório do executável. Um exemplo de arquivo pode ser visualizado em [input.txt](https://github.com/ddvargas/IngressosDaBroduai/blob/master/input.txt).

Iniciar a execução do programa e aguardar a finalização dele. Ao final da execução pode-se visualizar os resultados da execução na saída padrão ou no arquivo de trace gerado.

Um exemplo de arquivo de trace pode ser visualizado em [trace.txt](https://github.com/ddvargas/IngressosDaBroduai/blob/master/trace.txt).

__Obs.:__ O programa só executará em sistemas UNIX.


## Compilação
O programa foi desenvolvido na linguagem C, portando para sua compilação é necessário um compilador, que pode ser o [GCC](https://gcc.gnu.org/). Foi utilizado a biblioteca [pthread.h](https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread.h.html) desenvolvida para sistemas UNIX, portanto verifique sua existência no sistema. Há a possibilidade de usar o [CMake](https://cmake.org/) para configuração do projeto.

#### Usando apenas o GCC
Baixe o arquivo main.c, commpile usando o comando `gcc main.c -phtread`. Isso criará um executável `a.out`.

#### Usando o CMake
Baixe todo o projeto, execute e a saída será um executável `ingressos`.
