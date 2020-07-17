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

#define TAM_BUFFER_FILE 255

enum estado_lugar {
    LOCADO, VAZIO, EM_COMPRA
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

void *thread_evento(void *arg);

void *thread_cliente(void *args);

int main() {
    FILE *input;
    int num_eventos = 0, max_lotacao = 0;
    char buffer_read_input[TAM_BUFFER_FILE];
    char *linha;

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
        int lotacao = atoi(linha);
        eventos[num_eventos - 1].max_lotacao = lotacao;
        if (max_lotacao < lotacao) {
            max_lotacao = lotacao;
        }
        linha = strtok(NULL, "|");
        eventos[num_eventos - 1].valor_ingresso = atof(linha);
        linha = strtok(NULL, "|");
        eventos[num_eventos - 1].max_clientes_gerar = atoi(linha);
        eventos[num_eventos - 1].id = num_eventos - 1;
    }
    write_trace("Leitura arquivo input terminada\n");

    pthread_t tids[num_eventos][max_lotacao];
    for (int i = 0; i < num_eventos; i++) {
        printf("Inicializando evento %d\n", i);
        sem_init(&(eventos[i].mutext), 0, 1);
        sem_init(&(eventos[i].empty), 0, eventos[i].max_lotacao);
        sem_init(&(eventos[i].full), 0, 0);

        eventos[i].lugares = (STATUS *) malloc(sizeof(STATUS) * eventos[i].max_lotacao);

        for (int j = 0; j < eventos[i].max_lotacao; ++j) {
            eventos[i].lugares[j] = VAZIO;
            printf("Evento[%d][%d]: %d\n", i, j, eventos[i].lugares[j]);
        }
    }
    printf("Num eventos: %d\n", num_eventos);
    for (int i = 0; i < num_eventos; i++) {
        for (int j = 0; j < eventos[i].max_lotacao; ++j) {
            ARG args;
            args.id_evento = i;
            args.id_thread = j;
            if (pthread_create(&tids[i][j], NULL, thread_cliente, (void *) &args)) {
                printf("ERRO ao criar thread %d para evento %d", j, i);
            }
        }
    }

    for (int i = 0; i < num_eventos; ++i) {
        for (int j = 0; j < eventos[i].max_lotacao; ++j) {
            pthread_join(tids[i][j], NULL);
        }
    }

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


void *thread_cliente(void *args) {
    ARG *targ = (ARG *) args;

    printf("Thread cliente %d do evento %d processada\n", targ->id_thread, targ->id_evento);
    sleep(3);

//    free(args);
    pthread_exit(NULL);
}
