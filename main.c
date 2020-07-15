//
// Created by Daniel Vargas on 10/07/2020.
//

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TAM_BUFFER_FILE 255

struct evento {
    int id;
    char nome[100];
    int max_lotacao;
    float valor_ingresso;
    int max_clientes_gerar;
} typedef EVENTO;

FILE *trace;
EVENTO *eventos;

void write_trace(char message[]);

int get_randon(int max_value);

void thread_evento(void *args);

void thread_cliente(void *args);

int main() {
    FILE *input;

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
    eventos = malloc(sizeof(EVENTO));
    int num_eventos = 0;
    char buffer_read_input[TAM_BUFFER_FILE];
    char *linha;

    //processamento
    write_trace("Lendo arquivo de input\n");
    srand(time(NULL));
    while (!feof(input)) {
        fgets(buffer_read_input, TAM_BUFFER_FILE, input);
        linha = strtok(buffer_read_input, "|");
        eventos = realloc(eventos, ++num_eventos);
        //para cada linha, extrair os parâmetros
        strcpy(eventos[num_eventos - 1].nome, linha);
        linha = strtok(NULL, "|");
        eventos[num_eventos - 1].max_lotacao = atoi(linha);
        linha = strtok(NULL, "|");
        eventos[num_eventos - 1].valor_ingresso = atof(linha);
        linha = strtok(NULL, "|");
        eventos[num_eventos - 1].max_clientes_gerar = atoi(linha);
        eventos[num_eventos - 1].id = num_eventos - 1;
    }
    write_trace("Leitura arquivo input terminada\n");


    pthread_t tids[num_eventos];
    write_trace("Main criando threads de eventos\n");
    for (int i = 0; i < num_eventos; i++) {
        if (pthread_create(&tids[i], NULL, thread_evento, (void *) &eventos[i].id)) {
            printf("Erro ao criar thread do evento %d", i);
            exit(-1);
        }
    }
    pthread_exit(NULL);
    //TODO: para cada evento lançar as threads
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
    if (max_value == NULL) {
        return rand();
    } else {
        if (max_value == 0) {
            return 0;
        }
    }
    return rand() % max_value;
}

void thread_evento(void *args) {
    int *id_evento = (int *) args;

    printf("Id evento: %d", *id_evento);

    printf(", Max clientes gerar: %d", eventos[*id_evento].max_clientes_gerar);
    //TODO: Alocar o vetor de lugares disponíveis para o evento globalmente


    pthread_t clientes[eventos[*id_evento].max_clientes_gerar];
    for (int i = 0; i < 5; ++i) {
        //TODO: gerar as threads de clientes para o evento
        pthread_create(&clientes[i], NULL, thread_cliente, (void *) args);
    }


    pthread_exit(NULL);
}

void thread_cliente(void *args) {

}
//TODO: criar função de cliente