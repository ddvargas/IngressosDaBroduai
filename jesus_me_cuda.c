//
// Created by Daniel Vargas on 10/07/2020.
//

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

#define TAM_BUFFER_FILE 255
#define MAX_SLEEP_SOLICITAR_INGRESSO 20
#define MAX_SLEEP_AUTORIZACAO_PAGAMENTO 20

enum estado_lugar {
    VENDIDO, VAZIO, EM_COMPRA
} typedef STATUS;

struct evento {
    int id;
    char nome[100];
    int max_lotacao;
    float valor_ingresso;
    int max_clientes_gerar;
    STATUS *lugares;
    sem_t mutext, empty, full;
} typedef EVENTO;

struct thread_arg {
    int id_evento;
    int id_thread;
} typedef ARG;

FILE *trace;
EVENTO *eventos;
sem_t mutex; //mutex para realocações de memoria

void write_trace(char message[]);

int get_randon(int max_value);

void *thread_cliente(void *args);

int solicitar_ingresso(int id_evento);

bool autorizar_pagamento();

bool confirmar_compra_evento(int id_evento, int id_lugar);

void liberar_lugar(int id_evento, int id_lugar);

int main() {
    FILE *input;
    int num_eventos = 0, max_clientes = 0;
    char buffer_read_input[TAM_BUFFER_FILE];
    char *linha;
    ARG *args = NULL;

    input = fopen("input.txt", "r");
    trace = fopen("trace.txt", "a");

    if (input == NULL) {
        printf("Erro ao abrir arquivo de input\n");
        exit(1);
    }
    if (trace == NULL) {
        printf("Erro ao abrir arquivo de trace\n");
        exit(1);
    }

    //Novas alocações de memória
    eventos = NULL;


    //processamento
    write_trace("Lendo arquivo de input\n");
    srand(time(NULL));
    sem_init(&mutex, 0, 1);
    while (!feof(input)) {
        fgets(buffer_read_input, TAM_BUFFER_FILE, input);
        linha = strtok(buffer_read_input, "|");
        eventos = (EVENTO *) realloc(eventos, ++num_eventos);
        //para cada linha, extrair os parâmetros
        strcpy(eventos[num_eventos - 1].nome, linha);
        linha = strtok(NULL, "|");
        eventos[num_eventos - 1].max_lotacao = atoi(linha);

        linha = strtok(NULL, "|");
        eventos[num_eventos - 1].valor_ingresso = atof(linha);
        linha = strtok(NULL, "|");
        int max_cli = atoi(linha);
        eventos[num_eventos - 1].max_clientes_gerar = max_cli;
        if (max_clientes < max_cli) {
            max_clientes = max_cli;
        }
        eventos[num_eventos - 1].id = num_eventos - 1;
    }
    write_trace("Leitura arquivo input terminada\n");

    pthread_t tids[num_eventos][max_clientes];
    for (int i = 0; i < num_eventos; i++) {
        fprintf(trace, "Inicializando evento %d", i);
        printf("Inicializando evento %d\n", i);
        sem_init(&(eventos[i].mutext), 0, 1);
        sem_init(&(eventos[i].empty), 0, eventos[i].max_lotacao);
        sem_init(&(eventos[i].full), 0, 0);

        fprintf(trace, "Alocando memória para vetor de lugares do evento %d - Tamanho %d\n", i, eventos[i].max_lotacao);
        printf("Alocando memória para vetor de lugares do evento %d - Tamanho %d\n", i, eventos[i].max_lotacao);
        eventos[i].lugares = (STATUS *) malloc(sizeof(STATUS) * eventos[i].max_lotacao);

        for (int j = 0; j < eventos[i].max_lotacao; ++j) {
            eventos[i].lugares[j] = VAZIO;
            printf("Evento[%d][%d]: %d\n", i, j, eventos[i].lugares[j]);
        }
    }
    fprintf("Num eventos: %d\n", num_eventos);
    printf("Num eventos: %d\n", num_eventos);
    for (int i = 0; i < num_eventos; i++) {
        for (int j = 0; j < eventos[i].max_clientes_gerar; ++j) {
            args = (ARG *) malloc(sizeof(ARG));
            args->id_evento = i;
            args->id_thread = j;
            if (pthread_create(&tids[i][j], NULL, thread_cliente, (void *) args)) {
                printf("ERRO ao criar thread %d para evento %d", j, i);
                fprintf("ERRO ao criar thread %d para evento %d", j, i);
            }
        }
    }

    for (int i = 0; i < num_eventos; ++i) {
        for (int j = 0; j < eventos[i].max_lotacao; ++j) {
            pthread_join(tids[i][j], NULL);
        }
    }

    fprintf(trace, "Execução finalizada\n\n");

    fclose(input);
    fclose(trace);
    exit(0);
}

/**
 * Escreve no arquivo de trace a mesaagem passada
 * @param trace arquivo de escrita
 * @param message
 */
void write_trace(char message[]) {
    //TODO: modificar para que o trace seja global e a função só receba a mensagem
    if (trace != NULL && message != NULL) {
        fprintf(trace, message);
    }
}

/**
 * Retorna um numero aleatório entre 0 e maximo
 * @param max_value numero maximo do aleatorio
 * @return número aleatório
 */
int get_randon(int max_value) {
    if (max_value <= 0) {
        return 0;
    }
    return rand() % max_value;
}

/**
 * Implementa o comportamento de um cliente
 * @param args identificador de thread e de evento
 * @return nada
 */
void *thread_cliente(void *args) {
    ARG *targ = (ARG *) args;

    //solicitar um ingresso
    //se tem lugar no espetáculo
    //sleep(get_ramdom(MAX_SLEEP))
    //efetuar o pagamento
    //aguardar atorização da operadora do cartão
    //se autorizada
    //confirmar compra no evento (setar como vendido o lugar)
    //senão liberar lugar
    //senão recomendar outro espetaculo
    //se cliente quiser outro espetáculo
    //trocar id da thread para iniciar o processo de compra novamente em outro evento

    sleep(get_randon(10));
    printf("Thread cliente %d do evento %d processada\n", targ->id_thread, targ->id_evento);
    fprintf(trace, "Thread cliente %d do evento %d processada\n", targ->id_thread, targ->id_evento);

    free(args);
    pthread_exit(NULL);
}

/**
 * Verifica o vetor de lugares do evento e marca ele como EM_COMPRA
 * @param id_evento id do evento que se quer comprar um ingresso
 * @return o lugar escolhido, senão retorna -1 para nenhum lugar disponível ou retorna -2 se algum erro acontecer
 */
int solicitar_ingresso(int id_evento) {
    int retorno = -1;
    if (id_evento >= 0) {
        sem_wait(&eventos[id_evento].mutext);
        for (int i = 0; i < eventos[id_evento].max_lotacao; ++i) {
            if (eventos[id_evento].lugares[i] == VAZIO) {
                retorno = i;
            }
        }
        sem_post(&eventos[id_evento].mutext);
        return retorno;
    }
    return -2;
}

/**
 * Função que simula a operação de aprovação do pagamento por parte da operadora do cartão
 * @return true se aprovado, senão return false
 */
bool autorizar_pagamento() {
    sleep(get_randon(MAX_SLEEP_AUTORIZACAO_PAGAMENTO));
    if (get_randon(1) == 1) {
        return true;
    }
    return false;
}

/**
 * Função que confirma a compra de um lugar em um evento
 * @param id_evento que se deseja confirmar a compra
 * @param id_lugar id do lugar que se deseja confirmar a compra nesse evento
 * @return true se foi confirmada, senão retorna false
 */
bool confirmar_compra_evento(int id_evento, int id_lugar) {
    if (id_evento < 0 && id_lugar < 0) {
        return false;
    }
    bool retorno = false;
    sem_wait(&eventos[id_evento].mutext);
    if (eventos[id_evento].lugares[id_lugar] == EM_COMPRA) {
        eventos[id_evento].lugares[id_lugar] = VENDIDO;
        retorno = true;
    }
    sem_post(&eventos[id_evento].mutext);
    return retorno;
}

/**
 * Libera um lugar em um evento marcando o como VAZIO
 * @param id_evento que se quer liberar o lugar
 * @param id_lugar que se quer liberar
 */
void liberar_lugar(int id_evento, int id_lugar) {
    if (id_evento >= 0 && id_lugar >= 0) {
        sem_wait(&eventos[id_evento].mutext);
        eventos[id_evento].lugares[id_lugar] = VAZIO;
        sem_post(&eventos[id_evento].mutext);
    }
}
