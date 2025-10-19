/*
Nome:Caio Bastos, João Remédios, Rafael Pedra
RA:22402182,22406565,22403613
*/
//importação das blibliotecas
#include <stdio.h>
#include <stdlib.h>
// Delcaração da estrutura usada nos dados
typedef struct dados
{
    //Aqui estão todos os ids dos arquivos
    int idProcesso;
    int tempoEntrada;
    int tempoIO;
    int tempoProcessamento;
    int prioridade;
}dados;
// declaração da estrutura do nó
typedef struct no
{
    dados processos;
    struct no *proximo;
}no;

//Entrada da cabeça do nó na fila
no *entrar(no *cabeca, dados process)
{
    //Alocação de memória no nó
    no *novoNo=malloc(sizeof(no));
    if (novoNo==NULL)
    {
        printf("nao foi possivel alocar o no\n");
        exit(1);
    }
    novoNo->processos=process;
    novoNo->proximo=cabeca;
    return novoNo;
}
// Função pro nó entrar no fim da fila, para os processos que ainda tem tempo de processamento sobrando
no *entrar_fim(no *cabeca, dados process)
{
    no *novoNo=malloc(sizeof(no));
    if (novoNo==NULL)
    {
        printf("nao foi possivel alocar o no\n");
        exit(1);
    }
    novoNo->processos=process;
    novoNo->proximo=NULL;

    //se a lista ta só a cabeca
    if(cabeca==NULL)
    {
        return novoNo;
    }
    //Percorrer a lista até que a gente ache o ultimo nó
    no *temp=cabeca;
    while(temp->proximo!=NULL)
    {
        temp=temp->proximo;
    }
    //adição do novo nó
    temp->proximo=novoNo;
    return cabeca;
}

//função pra operar os nós da fila, usada para excluir os processos desnecessários
no *sair(no *cabeca, dados *process_remov)
{
    //Caso a fila esteja vazia
    if (cabeca == NULL)
    {
        return NULL;
    }

    //Declaração de variaveis
    no *excluir = cabeca;
    *process_remov = cabeca->processos; // Copia os dados do processo pra cabeça
    no *novaCabeca = cabeca->proximo;//Nova cabeça do nó
    free(excluir);
    //Retorna a nova cabeça do nó
    return novaCabeca;
}

//Função que ordena a prioridade por ordem decrescente(do menor para o maior)
int ordenar_prioridade(const void* a, const void* b)
{
    //Conversão dos dados void pra estrutura dos dados
    dados* nA = (dados*)a;
    dados* nB = (dados*)b;
    int sub = nA->prioridade - nB->prioridade;
    //checagem de conflito caso a subtração seja 0
    if (sub == 0)
    {
        return nA->tempoEntrada - nB->tempoEntrada;
    }
    //Aqui a função retorna a subtração 
    //Se B é maior q A, o resultado é positivo e B vem antes
    //Se A é maior q B, o resultado é negativo e A vem antes
    //Essa função vai ser usada futuramente com a outra
    return sub;
}

//Ordenação dos algoritimos usando bubble sort
void ordenar_lista(no* cabeca)
{
    if (cabeca == NULL) {
        return;
    }

    //Declaração de variaveis 
    int trocado;
    no* ptr1;
    no* lptr = NULL;

    //loop pra ordenação
    do
    {
        trocado = 0;
        ptr1 = cabeca;
        while (ptr1->proximo!= NULL)
        {
            //compara o processo atual com o proximo
            if (ordenar_prioridade(&ptr1->processos, &ptr1->proximo->processos) > 0)
            {
                //ordena os processos usando uma variável temporária com um loop
                dados temp = ptr1->processos;
                ptr1->processos = ptr1->proximo->processos;
                ptr1->proximo->processos = temp;
                trocado = 1;
            }
            ptr1 = ptr1->proximo;
        }
        lptr = ptr1;
    } while (trocado);
}