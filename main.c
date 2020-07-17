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
    int num_eventos = 0;
    char buffer_read_input[TAM_BUFFER_FILE];
    char *linha;
    pthread_t *tids = NULL;
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
        eventos[num_eventos - 1].max_clientes_gerar = atoi(linha);
        eventos[num_eventos - 1].id = num_eventos - 1;
    }
    write_trace("Leitura arquivo input terminada\n");

    write_trace("Main criando threads de eventos\n");
    for (int i = 0; i < num_eventos; i++) {
        //TODO: mutex nos realoc de tids e args

        sem_wait(&mutex);
        tids = (pthread_t *) realloc(tids, i + 1);
        args = (ARG *) realloc(args, i + 1);
        sem_post(&mutex);

        args[i].id_evento = i;
        args[i].id_thread = i + 1;
        printf("Criando thread %d para evento\n", i);
        if (pthread_create(&(tids[i]), NULL, thread_evento, (void *) &(args[i]))) {
            printf("Erro ao criar thread do evento %d", i);
//            exit(-1);
        }
    }
    printf("Num eventos: %d\n", num_eventos);
    for (int j = 0; j < num_eventos; ++j) {
        pthread_join(tids[j], NULL);
    }

    free(tids);
    free(args);
    free(eventos);
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

void *thread_evento(void *arg) {
    ARG *targ = (ARG *) arg;
    ARG *args = NULL;
    pthread_t *tids = NULL;


    printf("Thread %d do evento %d foi criada\n", targ->id_thread, targ->id_evento);

    //cada thread vai iniciar seus mutexes dos seus eventos
    sem_init(&eventos[targ->id_evento].mutext, 0, 1);
    sem_init(&eventos[targ->id_evento].empty, 0, eventos[targ->id_evento].max_lotacao);
    sem_init(&eventos[targ->id_evento].full, 0, 0);
    //TODO: aqui colocar a inicialização de um mutex particular?

    printf("Thread %d do evento %d iniciou semáforos\n", targ->id_thread, targ->id_evento);


    // argumentos para que seja feito o free após
    for (int i = 0; i < eventos[targ->id_evento].max_clientes_gerar; i++) {
        sem_wait(&mutex);
        tids = (pthread_t *) realloc(tids, i + 1);
        args = (ARG *) realloc(args, i + 1);
        sem_post(&mutex);

        printf("Evento %d criando thread %d para cliente\n", targ->id_evento, i);
        if (pthread_create(&(tids[i]), NULL, thread_cliente, (void *) args)) {
            printf("ERRO ao criar threads dos clientes evento %d", i);
        }
    }

    for (int k = 0; k < eventos[targ->id_evento].max_clientes_gerar; ++k) {
        pthread_join(tids[k], NULL);
    }


    sem_wait(&eventos[targ->id_evento].mutext);
    free(&eventos[targ->id_evento].lugares);
    sem_post(&eventos[targ->id_evento].mutext);
    free(tids);
    free(args);
    pthread_exit(NULL);
}

void *thread_cliente(void *args) {
    ARG *targ = (ARG *) args;

    printf("Thread cliente %d do evento %d processada\n", targ->id_thread, targ->id_evento);
    sleep(1);

    pthread_exit(NULL);
}
