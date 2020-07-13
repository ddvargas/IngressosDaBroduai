//
// Created by Daniel Vargas on 10/07/2020.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAM_BUFFER_FILE 255

struct evento {
    char nome[100];
    int max_lotacao;
    float valor_ingresso;
    int max_clientes_gerar;
} typedef EVENTO;

int main() {
    FILE *input;
    FILE *trace;
    EVENTO *eventos = malloc(sizeof(EVENTO));
    int num_eventos = 0;

    input = fopen("input.txt", "r");
    trace = fopen("trace.txt", "a");

    if (input != NULL) {
        if (trace != NULL) {
            char buffer_read_input[TAM_BUFFER_FILE];
            char *linha;
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
            }

//            for (int i = 0; i < num_eventos; i++) {
//                printf("Evento %d\n", i);
//                printf("nome: %s\n", eventos[i].nome);
//                printf("Lotacao maxima: %d\n", eventos[i].max_lotacao);
//                printf("ingressos: %f\n", eventos[i].valor_ingresso);
//                printf("num clientes: %d\n", eventos[i].max_clientes_gerar);
//            }



            //TODO:
        } else {
            printf("Erro ao abrir arquivo de trace\n");
        }
    } else {
        printf("Erro ao abrir arquivo de input\n");
    }
}