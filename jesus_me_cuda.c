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
#define MAX_SLEEP_SOLICITAR_INGRESSO 2
#define MAX_SLEEP_AUTORIZACAO_PAGAMENTO 2

enum estado_lugar {
    VENDIDO = 1, VAZIO, EM_COMPRA
} typedef STATUS;

struct relat {
    int pagamento_nao_autorizado;
    int recusa_recomendacao;
    int aceita_recomendacao;
    int indisp_total;
    int compra_nao_confirmada;
} typedef RELATORIO;

struct evento {
    char *nome;
    short unsigned int max_lotacao;
    float valor_ingresso;
    int max_clientes_gerar;
    STATUS *lugares;
    sem_t mutex, mutex_rel;
    RELATORIO *relatorio;
} typedef EVENTO;

struct thread_arg {
    int id_evento;
    int id_thread;
    bool relancada;
} typedef ARG;

FILE *trace;
EVENTO *eventos;
int num_eventos = 0;


int get_randon(int max_value);

void *thread_cliente(void *args);

int solicitar_ingresso(EVENTO *evento);

bool autorizar_pagamento();

bool confirmar_compra_evento(EVENTO *evento, int id_lugar);

void liberar_lugar(EVENTO *evento, int id_lugar);

int recomendacao();

EVENTO *get_evento(int id_evento);

void relatorio();


int main() {
    FILE *input;
    int max_clientes = 0;
    char buffer_read_input[TAM_BUFFER_FILE];
    char *linha;
    ARG *args = NULL;

    input = fopen("input.txt", "r");
    trace = fopen("trace.txt", "a");

    if (input == NULL) {
        printf("ERRO ao abrir arquivo de input\n");
        exit(1);
    }
    if (trace == NULL) {
        printf("ERRO ao abrir arquivo de trace\n");
        exit(1);
    }

    //Novas alocações de memória
    eventos = NULL;


    //processamento
    fprintf(trace, "INFO - Lendo arquivo de input\n");
    printf("INFO - Lendo arquivo de input\n");
    srand(time(NULL));
    while (fgets(buffer_read_input, TAM_BUFFER_FILE, input)) {
        if (buffer_read_input[0] != '\n') {
            linha = strtok(buffer_read_input, "|");
            if (linha != NULL) {
                eventos = (EVENTO *) realloc(eventos, sizeof(EVENTO) * ++num_eventos);
                eventos[num_eventos - 1].nome = (char *) malloc(sizeof(char) * strlen(linha));
                strcpy(eventos[num_eventos - 1].nome, linha);
                linha = strtok(NULL, "|");
            } else {
                printf("ERRO - Arquivo de input inconsistente\n");
                fprintf(trace, "ERRO - Arquivo de input inconsistente\n");
                exit(-1);
            }

            if (linha != NULL) {
                //para cada linha, extrair os parâmetros
                eventos[num_eventos - 1].max_lotacao = atoi(linha);
                linha = strtok(NULL, "|");
            } else {
                printf("ERRO - Arquivo de input inconsistente\n");
                fprintf(trace, "ERRO - Arquivo de input inconsistente\n");
                exit(-1);
            }

            if (linha != NULL) {
                eventos[num_eventos - 1].valor_ingresso = atof(linha);
                linha = strtok(NULL, "|");
            } else {
                printf("ERRO - Arquivo de input inconsistente\n");
                fprintf(trace, "ERRO - Arquivo de input inconsistente\n");
                exit(-1);
            }

            if (linha != NULL) {
                int max_cli = atoi(linha);
                eventos[num_eventos - 1].max_clientes_gerar = max_cli;
                if (max_clientes < max_cli) {
                    max_clientes = max_cli;
                }
            } else {
                printf("ERRO - Arquivo de input inconsistente\n");
                fprintf(trace, "ERRO - Arquivo de input inconsistente\n");
                exit(-1);
            }
        }
    }
    fprintf(trace, "INFO - Leitura arquivo input terminada\n");
    printf("INFO - Leitura arquivo input terminada\n");

    pthread_t tids[num_eventos][max_clientes];
    for (int i = 0; i < num_eventos; i++) {
        fprintf(trace, "INFO - Inicializando evento %d\n", i);
        printf("INFO - Inicializando evento %d\n", i);
        sem_init(&(eventos[i].mutex), 0, 1);
        sem_init(&(eventos[i].mutex_rel), 0, 1);

        printf("INFO - Alocando memória para vetor de lugares do evento %d (tamanho %d)\n", i, eventos[i].max_lotacao);
        eventos[i].lugares = (STATUS *) malloc(sizeof(STATUS) * eventos[i].max_lotacao);

        for (int j = 0; j < eventos[i].max_lotacao; j++) {
            eventos[i].lugares[j] = VAZIO;
            printf("Evento[%d][%d]: %d\n", i, j, eventos[i].lugares[j]);
        }

        eventos[i].relatorio = malloc(sizeof(RELATORIO));
        eventos[i].relatorio->pagamento_nao_autorizado = 0;
        eventos[i].relatorio->indisp_total = 0;
        eventos[i].relatorio->recusa_recomendacao = 0;
        eventos[i].relatorio->compra_nao_confirmada = 0;
        eventos[i].relatorio->aceita_recomendacao = 0;
    }
    fprintf(trace, "INFO - Num eventos: %d\n", num_eventos);
    printf("INFO - Num eventos: %d\n", num_eventos);
    fprintf(trace, "INFO - Lançando threads\n");
    printf("INFO - Lançando threads\n");
    for (int i = 0; i < num_eventos; i++) {
        for (int j = 0; j < eventos[i].max_clientes_gerar; j++) {
            args = (ARG *) malloc(sizeof(ARG));
            args->id_evento = i;
            args->id_thread = NULL;
            args->relancada = false;
            if (pthread_create(&tids[i][j], NULL, thread_cliente, (void *) args)) {
                printf("ERRO - ao criar thread %d para evento %d", j, i);
                fprintf(trace, "ERRO - ao criar thread %d para evento %d", j, i);
            } else {
                printf("LANCAMENTO - %d\n", tids[i][j]);
                fprintf(trace, "LANCAMENTO - %d\n", tids[i][j]);
            }
        }
    }

    for (int i = 0; i < num_eventos; i++) {
        for (int j = 0; j < eventos[i].max_clientes_gerar; j++) {
            pthread_join(tids[i][j], 0);
        }
    }

    fprintf(trace, "INFO - Execução finalizada\n");
    printf("INFO - Execução finalizada\n");
    relatorio();

    fclose(input);
    fclose(trace);
    for (int k = 0; k < num_eventos; ++k) {
        free(eventos[k].nome);
        free(eventos[k].lugares);
        free(eventos[k].relatorio);
    }
    free(eventos);
    exit(0);
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
    return rand() % (max_value + 1);
}

/**
 * Implementa o comportamento de um cliente
 * @param args identificador de thread e de evento
 * @return nada
 */
void *thread_cliente(void *args) {
    ARG *targ = (ARG *) args;
    EVENTO *meu_evento;
    int meu_lugar_evento;

    meu_evento = get_evento(targ->id_evento);
    if (!targ->relancada) {
        targ->id_thread = (int) pthread_self(); //recuperação do ID único da thread
    }

    printf("INFO - Cliente %d solicitando ingresso no evento %s\n",
           targ->id_thread, meu_evento->nome);
    fprintf(trace, "INFO - Cliente %d solicitando ingresso no evento %s\n",
            targ->id_thread, meu_evento->nome);

    meu_lugar_evento = solicitar_ingresso(meu_evento);


    if (meu_lugar_evento >= 0) {
        printf("INFO - Cliente %d do evento %s solicitou lugar %d\n",
               targ->id_thread, meu_evento->nome, meu_lugar_evento);
        fprintf(trace, "INFO - Cliente %d do evento %s solicitou lugar %d\n",
                targ->id_thread, meu_evento->nome, meu_lugar_evento);

        sleep(MAX_SLEEP_SOLICITAR_INGRESSO);

        printf("INFO - Cliente %d do evento %s aguardando autorização de pagamento\n",
               targ->id_thread, meu_evento->nome);
        fprintf(trace, "INFO - Cliente %d do evento %s aguardando autorização de pagamento\n",
                targ->id_thread, meu_evento->nome);

        if (autorizar_pagamento()) {
            printf("PAGAMENTO CONFIRMADO - Pagamento do cliente %d do evento %s no lugar %d foi autorizada\n",
                   targ->id_thread, meu_evento->nome, meu_lugar_evento);
            fprintf(trace,
                    "PAGAMENTO CONFIRMADO - Pagamento da compra do cliente %d do evento %s no lugar %d foi autorizada\n",
                    targ->id_thread, meu_evento->nome, meu_lugar_evento);


            if (confirmar_compra_evento(meu_evento, meu_lugar_evento)) {
                printf("COMPRA CONFIRMADA - Compra do cliente %d no evento %s confirmada no lugar %d\n",
                       targ->id_thread, meu_evento->nome, meu_lugar_evento);
                fprintf(trace, "COMPRA CONFIRMADA - Compra do cliente %d no evento %s confirmada no lugar %d\n",
                        targ->id_thread, meu_evento->nome, meu_lugar_evento);
            } else {
                printf("COMPRA RECUSADA - Compra do cliente %d no evento %s não confirmada no lugar %d\n",
                       targ->id_thread, meu_evento->nome, meu_lugar_evento);
                fprintf(trace,
                        "COMPRA RECUSADA - Compra do cliente %d no evento %s não confirmada no lugar %d\n",
                        targ->id_thread, meu_evento->nome, meu_lugar_evento);

                sem_wait(&meu_evento->mutex_rel);
                meu_evento->relatorio->compra_nao_confirmada++;
                sem_post(&meu_evento->mutex_rel);
                liberar_lugar(meu_evento, meu_lugar_evento);
            }

        } else {
            printf("PAGAMENTO RECUSADO - Pagamento do cliente %d do evento %s no lugar %d não autorizada\n",
                   targ->id_thread, meu_evento->nome, meu_lugar_evento);
            fprintf(trace,
                    "PAGAMENTO RECUSADO - Pagamento do cliente %d do evento %s no lugar %d não autorizada\n",
                    targ->id_thread, meu_evento->nome, meu_lugar_evento);

            sem_wait(&meu_evento->mutex_rel);
            eventos[targ->id_evento].relatorio->pagamento_nao_autorizado++;
            sem_post(&meu_evento->mutex_rel);
            liberar_lugar(meu_evento, meu_lugar_evento);
        }
    } else {
        int new_id_evento = recomendacao();

        if (meu_lugar_evento == -1 && new_id_evento == -1) {
            sem_wait(&meu_evento->mutex_rel);
            meu_evento->relatorio->indisp_total++;
            sem_post(&meu_evento->mutex_rel);
        }

        //recomendar outro espetáculo se a thread já não foi relançada
        if (!targ->relancada) {
            pthread_t tid;

            if (new_id_evento >= 0) {
                if (get_randon(1)) {
                    printf("RECOMENDACAO SEGUIDA - Cliente %d aceitou recomendação do evento %s\n",
                           targ->id_thread, get_evento(new_id_evento)->nome);
                    fprintf(trace, "RECOMENDACAO SEGUIDA - Cliente %d aceitou recomendação do evento %s\n",
                            targ->id_thread, get_evento(new_id_evento)->nome);


                    sem_wait(&meu_evento->mutex_rel);
                    meu_evento->relatorio->aceita_recomendacao++;
                    sem_post(&meu_evento->mutex_rel);
                    ARG *new_arg = (ARG *) malloc(sizeof(ARG));
                    new_arg->id_thread = targ->id_thread;
                    new_arg->id_evento = new_id_evento;
                    new_arg->relancada = true;
                    pthread_create(&tid, NULL, thread_cliente, (void *) new_arg);
                    pthread_join(tid, 0);
                } else {
                    sem_wait(&meu_evento->mutex_rel);
                    eventos[targ->id_evento].relatorio->recusa_recomendacao++;
                    sem_post(&meu_evento->mutex_rel);
                    printf("RECOMENDACAO IGNORADA - Cliente %d recusou a recomendação do evento %s\n",
                           targ->id_thread, get_evento(new_id_evento)->nome);
                    fprintf(trace,
                            "RECOMENDACAO IGNORADA - Cliente %d recusou a recomendação do evento %s\n",
                            targ->id_thread, get_evento(new_id_evento)->nome);
                }
            }
        }

    }

    fprintf(trace, "INFO - Thread cliente %d do evento %s processada\n",
            targ->id_thread, meu_evento->nome);

    free(args);
    pthread_exit(0);
}

/**
 * Verifica o vetor de lugares do evento e marca ele como EM_COMPRA
 * @param id_evento id do evento que se quer comprar um ingresso
 * @return o lugar escolhido, senão retorna -1 para nenhum lugar disponível ou retorna -2 se algum erro acontecer
 */
int solicitar_ingresso(EVENTO *evento) {
    int retorno = -1;
    if (evento != NULL) {
        sem_wait(&evento->mutex);
        for (int i = 0; i < evento->max_lotacao; i++) {
            if (evento->lugares[i] == VAZIO) {
                evento->lugares[i] = EM_COMPRA;
                retorno = i;
                break;
            }
        }
        sem_post(&evento->mutex);
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
    if (get_randon(1)) {
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
bool confirmar_compra_evento(EVENTO *evento, int id_lugar) {
    if (evento == NULL && id_lugar < 0) {
        return false;
    }
    bool retorno = false;
    sem_wait(&evento->mutex);
    if (evento->lugares[id_lugar] == EM_COMPRA) {
        evento->lugares[id_lugar] = VENDIDO;
        retorno = true;
    }
    sem_post(&evento->mutex);
    return retorno;
}

/**
 * Libera um lugar em um evento marcando o como VAZIO
 * @param id_evento que se quer liberar o lugar
 * @param id_lugar que se quer liberar
 */
void liberar_lugar(EVENTO *evento, int id_lugar) {
    if (evento != NULL && id_lugar >= 0) {
        sem_wait(&evento->mutex);
        if (evento->lugares[id_lugar] == EM_COMPRA) {
            evento->lugares[id_lugar] = VAZIO;
        }
        sem_post(&evento->mutex);
    }
}

/**
 * Sistema de recomendação de um evento
 * @return id do evento recomendado, senão retorna -1.
 */
int recomendacao(){
    //TODO: deixar a recomendação menos procedural, não indo do evento 0 ao max
    int id_evento = -1;
    for (int i = 0; i < num_eventos; i++) {
        for (int j = 0; j < eventos[i].max_lotacao; j++) {
            if (eventos[i].lugares[j] == VAZIO){
                id_evento = i;
                break;
            }
            if (id_evento != -1){
                break;
            }
        }
    }
    return id_evento;
}

/**
 * Imprime o relatório da execução
 */
void relatorio(){
    printf("\n######## Relatório ########\n");
    fprintf(trace, "\n######## Relatório ########\n");
    for (int i = 0; i < num_eventos; i++) {
        int vendidos = 0;
        printf("Evento: %s\nLugares: ", eventos[i].nome);
        fprintf(trace, "Evento: %s\nLugares: ", eventos[i].nome);
        for (int j = 0; j < eventos[i].max_lotacao; j++) {
            switch (eventos[i].lugares[j]) {
                case VENDIDO:
                    vendidos++;
                    printf("[%d]:VENDIDO  ", j);
                    fprintf(trace, "[%d]:VENDIDO  ", j);
                    break;
                case VAZIO:
                    printf("[%d]:VAZIO  ", j);
                    fprintf(trace, "[%d]:VAZIO  ", j);
                    break;
                case EM_COMPRA:
                    printf("[%d]:EM_COMPRA  ", j);
                    fprintf(trace, "[%d]:EM_COMPRA  ", j);
                    break;
            }
        }
        printf("\n(%d/%d) vendidos - R$%.2f\n",
               vendidos, eventos[i].max_lotacao, (vendidos * eventos[i].valor_ingresso));
        fprintf(trace, "\n(%d/%d) vendidos - R$%.2f\n",
                vendidos, eventos[i].max_lotacao, (vendidos * eventos[i].valor_ingresso));
        printf("%d clientes recusaram a recomendação\n%d clientes aceitaram a recomendação\n"
               "%d não tiveram lugares por indisponibilidade total\n"
               "%d tiveram pagamento recusado pela operadora\n%d tiveram a compra não confirmada\n\n",
               eventos[i].relatorio->recusa_recomendacao, eventos[i].relatorio->aceita_recomendacao,
               eventos[i].relatorio->indisp_total,
               eventos[i].relatorio->pagamento_nao_autorizado, eventos[i].relatorio->compra_nao_confirmada);
        fprintf(trace, "%d clientes recusaram a recomendação\n%d clientes aceitaram a recomendação\n"
                       "%d não tiveram lugares por indisponibilidade total\n"
                       "%d tiveram pagamento recusado pela operadora\n%d tiveram a compra não confirmada\n\n",
                eventos[i].relatorio->recusa_recomendacao, eventos[i].relatorio->aceita_recomendacao,
                eventos[i].relatorio->indisp_total,
                eventos[i].relatorio->pagamento_nao_autorizado, eventos[i].relatorio->compra_nao_confirmada);
    }
}

/**
 * Retorna um evento do banco de eventos.
 * @param id_evento Id do evento que se quer.
 * @return O ponteiro do evento indicado pelo id solicitado, senão retorna NULL
 */
EVENTO *get_evento(int id_evento) {
    if (id_evento >= 0) {
        return &eventos[id_evento];
    }
    return NULL;
}