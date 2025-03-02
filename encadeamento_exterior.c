#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limits.h>
#include "lista_compartimentos.h"
#include "lista_clientes.h"
#include <stdlib.h>
#include "cliente.h"

int posicaoLivre(char *nome_arquivo_dados)
{
    FILE *arqDados = fopen(nome_arquivo_dados, "rb");
    int i = 0;
    while (1)
    {
        fseek(arqDados, (i * sizeof(Cliente)), SEEK_SET);
        Cliente *c = le_cliente(arqDados);
        if (c == NULL || c->status == LIBERADO)
        {
            free(c);
            break;
        }
        i++;
    }
    fclose(arqDados);
    return i;
}

int hash(int valor, char *nome_arquivo_hash)
{
    FILE *tab_hash = fopen(nome_arquivo_hash, "rb+");
    ListaCompartimentos *lComp = le_compartimentos(nome_arquivo_hash);
    int tam = lComp->qtd;
    fclose(tab_hash);
    return valor % tam;
}

void cria_hash(char *nome_arquivo_hash, int tam)
{
    FILE *tab_hash = fopen(nome_arquivo_hash, "wb");
    int i = 0;
    while (i < tam)
    {
        fseek(tab_hash, (i * sizeof(CompartimentoHash)), SEEK_SET);
        CompartimentoHash *comp = compartimento_hash(-1);
        salva_compartimento(comp, tab_hash);
        free(comp);
        i++;
    }
    fclose(tab_hash);
}

int busca(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
    FILE *arqDados = fopen(nome_arquivo_dados, "rb");
    FILE *tab_hash = fopen(nome_arquivo_hash, "rb");
    int end = hash(cod_cli, nome_arquivo_hash);
    fseek(tab_hash, end * (sizeof(CompartimentoHash)), SEEK_SET);
    fread(&end, sizeof(int), 1, tab_hash);
    fclose(tab_hash);

    while (end != -1)
    {
        fseek(arqDados, (end * sizeof(Cliente)), SEEK_SET);
        Cliente *c = le_cliente(arqDados);
        if (c->cod_cliente == cod_cli)
        {

            if (c->status == OCUPADO)
            {
                free(c);
                break;
            }
        }
        end = c->prox;
        free(c);
    }
    fclose(arqDados);
    return end;
}

int insere(int cod_cli, char *nome_cli, char *nome_arquivo_hash, char *nome_arquivo_dados, int num_registros)
{
    FILE *arqDados = fopen(nome_arquivo_dados, "rb+");
    Cliente *c = cliente(cod_cli, nome_cli, -1, OCUPADO);
    ListaCompartimentos *compartimentos = le_compartimentos(nome_arquivo_hash);
    int end = hash(cod_cli, nome_arquivo_hash);
    if (compartimentos->lista[end]->prox == -1)
    {
        int livre = posicaoLivre(nome_arquivo_dados);
        compartimentos->lista[end]->prox = livre;
        fseek(arqDados, (livre * sizeof(Cliente)), SEEK_SET);
        salva_cliente(c, arqDados);
        salva_compartimentos(nome_arquivo_hash, compartimentos);
        free(c);
        free(compartimentos);
        end = livre;
        fclose(arqDados);
        return end;
    }
    else
    {

        Cliente *aux;
        end = compartimentos->lista[end]->prox;
        free(compartimentos);
        int prox = end;
        int atual = -1;
        do
        {
     
            fseek(arqDados, (prox * sizeof(Cliente)), SEEK_SET);
            aux = le_cliente(arqDados);
            if (aux->cod_cliente == cod_cli)
            {
                if (aux->status == LIBERADO)
                {
                    c->prox = aux->prox;
                    fseek(arqDados, (prox * sizeof(Cliente)), SEEK_SET);
                    salva_cliente(c, arqDados);
                    fclose(arqDados);
                    free(c);
                    free(aux);
                    return prox;
                }
                fclose(arqDados);
                free(c);
                free(aux);
                return -1;
            }
            else
            {
                if (aux->status == LIBERADO)
                {
                    c->prox = aux->prox;
                    fseek(arqDados, (prox * sizeof(Cliente)), SEEK_SET);
                    salva_cliente(c, arqDados);
                    fclose(arqDados);
                    free(c);
                    free(aux);
                    return prox;
                }
            }
            atual = prox;
            prox = aux->prox;

        } while (prox != -1);
        int livre = posicaoLivre(nome_arquivo_dados);
        aux->prox = livre;
        fseek(arqDados, (atual * sizeof(Cliente)), SEEK_SET);
        salva_cliente(aux, arqDados);
        end = livre;
        fseek(arqDados, (end * sizeof(Cliente)), SEEK_SET);
        salva_cliente(c, arqDados);
        free(aux);
        free(c);
        fclose(arqDados);
        return livre;
    }
  
}

int exclui(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{

    FILE *arqDados = fopen(nome_arquivo_dados, "rb+");
    int end = busca(cod_cli, nome_arquivo_hash, nome_arquivo_dados);
    if (end == -1)
        return -1;
    fseek(arqDados, (end * sizeof(Cliente)), SEEK_SET);
    Cliente *cli = le_cliente(arqDados);
    cli->status = LIBERADO;
    fseek(arqDados, (end * sizeof(Cliente)), SEEK_SET);
    salva_cliente(cli, arqDados);
    free(cli);
    fclose(arqDados);

    return end;
}
