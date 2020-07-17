//
// Created by Daniel Vargas on 10/07/2020.
//

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <semaphore.h>

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
    sem_t mutex, empty, full;
} typedef EVENTO;

struct arg {
    int id_evento;
    int id_thread;
} typedef t_Args;

FILE *trace;
EVENTO *eventos;


int main() {
    FILE *input;
    int num
}