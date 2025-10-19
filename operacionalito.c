/*
Nome:Caio Bastos, João Remédios, Rafael Pedra
RA:22402182,22406565,22403613
*/
//Importação das blibliotecas necessarias
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //uso de sleep para exibição mais consistente
#include <stdbool.h> //para usar booleanos (true/false)

//Importação da estrutura de dados usadas
#include "fila.h"

//definição da função que lê o arquivo e retorna os processos
no *ler_arq(char *nome_arquivo)
{
    //Abertura do arquivo, assim conseguimos os dados dos processos
    FILE *arq = fopen(nome_arquivo, "r");
    if (arq == NULL)
    {
        printf("Nao foi possivel abrir o arquivo");
        return NULL;
    }
    //Declaração da fila e dos processos
    no *f_cabeca = NULL;
    dados temp_processo;

    //Leitura das linhas do arquivo
    while (fscanf(arq, "%d;%d;%d;%d;%d", &temp_processo.idProcesso, &temp_processo.tempoEntrada, &temp_processo.tempoIO, &temp_processo.tempoProcessamento, &temp_processo.prioridade) == 5)
    {
        //insere a cabeça no nó
        f_cabeca = entrar(f_cabeca, temp_processo);
    }
    //Fechamento do arquivo e retorno da cabeça
    fclose(arq);
    return f_cabeca;
}

//Função pra imprimir os nós contendo os processos
void imprimir(no *cabeca)
{
    //declaração do ponteiro temporario
    no *ptr = cabeca;
    while (ptr != NULL)
    {
        //iteração pra ele imprimir os dados
        printf("ID: %d, Entrada: %d, I/O: %d, Process.: %d, Prio: %d\n", ptr->processos.idProcesso, ptr->processos.tempoEntrada, ptr->processos.tempoIO, ptr->processos.tempoProcessamento, ptr->processos.prioridade);
        ptr = ptr->proximo;
    }
}

//Função principal
int main()
{
    // --- INICIALIZAÇÃO DAS ESTRUTURAS ---
    no *fila_novos = ler_arq("dados.txt");
    no *fila_prontos = NULL;
    no *fila_espera = NULL; // Fila para processos em I/O

    FILE *arquivo_saida = fopen("saida.txt", "w");
    if (arquivo_saida == NULL) {
        printf("Nao foi possivel criar o arquivo de saida.\n");
        return 1;
    }

    int ciclo_clock_global = 0;
    int processos_concluidos = 0;
    
    dados processo_na_cpu;
    dados processo_em_io;

    bool cpu_ocupada = false;
    bool io_ocupado = false;
    
    int fatia_cpu_usada = 0;
    const int FATIA_CPU_MAX = 3;
    
    int fatia_io_usada = 0;
    const int FATIA_IO_MAX = 6; // O dobro da fatia de processamento

    printf("Iniciando simulacao do Operacionalito...\n");

    // --- LOOP PRINCIPAL DO ESCALONADOR ---
    // O loop continua enquanto houver processos em qualquer uma das filas ou em execução
    while (fila_novos != NULL || fila_prontos != NULL || fila_espera != NULL || cpu_ocupada || io_ocupado)
    {
        // 1. ADMISSÃO DE NOVOS PROCESSOS
        // Verifica se algum processo na fila de novos pode entrar na fila de prontos
        no *iter = fila_novos;
        no *ant = NULL;
        while(iter != NULL) {
            if(iter->processos.tempoEntrada <= ciclo_clock_global) {
                dados p_admitido = iter->processos;
                fila_prontos = entrar(fila_prontos, p_admitido); // Adiciona na fila de prontos
                
                // Remove o processo da fila de novos
                if(ant == NULL) { // se for o primeiro
                    fila_novos = iter->proximo;
                    free(iter);
                    iter = fila_novos;
                } else {
                    ant->proximo = iter->proximo;
                    free(iter);
                    iter = ant->proximo;
                }
            } else {
                ant = iter;
                iter = iter->proximo;
            }
        }
        ordenar_lista(fila_prontos); // Garante a ordem de prioridade

        // 2. PROCESSAMENTO DE I/O
        if (io_ocupado) {
            processo_em_io.tempoIO--;
            fatia_io_usada++;

            // Verifica se o I/O terminou (por tempo ou pela fatia)
            if (processo_em_io.tempoIO <= 0 || fatia_io_usada >= FATIA_IO_MAX) {
                if (processo_em_io.tempoProcessamento > 0) {
                    // Se ainda precisa de CPU, volta para a fila de prontos
                    fila_prontos = entrar(fila_prontos, processo_em_io);
                    ordenar_lista(fila_prontos);
                } else if (processo_em_io.tempoIO > 0) {
                    // Se não precisa de CPU mas ainda precisa de I/O, volta para a fila de espera
                    fila_espera = entrar_fim(fila_espera, processo_em_io);
                } else {
                    // Processo terminou I/O e CPU, vai para o ciclo final de encerramento
                    processo_em_io.tempoProcessamento = 1; // 1 ciclo para encerrar
                    fila_prontos = entrar(fila_prontos, processo_em_io);
                    ordenar_lista(fila_prontos);
                }
                io_ocupado = false;
            }
        }
        // Se o I/O está livre e há processos esperando, inicia o próximo
        if (!io_ocupado && fila_espera != NULL) {
            fila_espera = sair(fila_espera, &processo_em_io);
            io_ocupado = true;
            fatia_io_usada = 0;
        }

        // 3. PROCESSAMENTO DE CPU
        if (cpu_ocupada) {
            processo_na_cpu.tempoProcessamento--;
            fatia_cpu_usada++;

            // Verifica se o processo terminou sua fatia de tempo ou o processamento total
            if (processo_na_cpu.tempoProcessamento <= 0 || fatia_cpu_usada >= FATIA_CPU_MAX) {
                if (processo_na_cpu.tempoIO > 0) {
                    // Se ainda tem I/O, vai para a fila de espera
                    fila_espera = entrar_fim(fila_espera, processo_na_cpu);
                } else if (processo_na_cpu.tempoProcessamento > 0) {
                    // Se ainda tem CPU (mas não I/O), volta para a fila de prontos
                    fila_prontos = entrar(fila_prontos, processo_na_cpu);
                    ordenar_lista(fila_prontos);
                } else {
                    // Processo finalizado
                    printf("Clock %d: Fim do processo %d\n", ciclo_clock_global + 1, processo_na_cpu.idProcesso);
                    fprintf(arquivo_saida, "%d;%d\n", ciclo_clock_global + 1, processo_na_cpu.idProcesso);
                    processos_concluidos++;
                }
                cpu_ocupada = false; // Libera a CPU
            }
        }

        // Se a CPU está livre e há processos na fila de prontos, escalona o próximo
        if (!cpu_ocupada && fila_prontos != NULL) {
            fila_prontos = sair(fila_prontos, &processo_na_cpu);
            cpu_ocupada = true;
            fatia_cpu_usada = 0;
            printf("Clock %d: Executando processo ID: %d (Prioridade: %d)\n", ciclo_clock_global, processo_na_cpu.idProcesso, processo_na_cpu.prioridade);
        }

        // 4. AVANÇO DO CLOCK
        ciclo_clock_global++;
        sleep(1); // Opcional: para visualizar a execução passo a passo
    }

    // --- CONCLUSÃO DO PROGRAMA ---
    printf("---------------------------------------------------\n");
    printf("Operacionalito terminado com %d processos concluidos.\n", processos_concluidos);
    printf("Arquivo de saida 'saida.txt' foi gerado.\n");
    
    fclose(arquivo_saida);

    return 0;
}