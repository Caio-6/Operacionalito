#include <stdio.h>
#include <stdlib.h>
typedef struct dados
{
    int idProcesso;
    int tempoEntrada;
    int tempoIO;
    int tempoProcessamento;
    int prioridade;
} dados;

typedef struct no
{
    dados processos;
    struct no *proximo;
} no;

no *entrar(no *cabeca, dados process);
no *entrar_fim(no *cabeca, dados process);
no *sair(no* cabeca, dados* process_remov);
void ordenar_lista(no* cabeca);