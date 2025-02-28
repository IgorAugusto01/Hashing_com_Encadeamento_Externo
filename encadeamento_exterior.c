#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limits.h>
#include "lista_compartimentos.h"
#include "lista_clientes.h"
#include <stdlib.h>
#include "cliente.h"

int buscaRecursiva(int end, int cod_cli, char *nome_arquivo_dados)
{
    if (end == -1)
        return -1;
    FILE *dados = fopen(nome_arquivo_dados, "rb");
    fseek(dados, (end * sizeof(Cliente)), SEEK_SET);
    Cliente *c = le_cliente(dados);
    fclose(dados);
    if (c->cod_cliente == cod_cli)
    {
        if (c->status == OCUPADO)
        {
            free(c);
            return end;
        }
        end = c->prox;
        free(c);
        return buscaRecursiva(end, cod_cli, nome_arquivo_dados);
    }
    end = c->prox;
    free(c);

    return buscaRecursiva(end, cod_cli, nome_arquivo_dados);
}

int hash(int valor, int tam)
{
    return valor % tam;
}

void cria_hash(char *nome_arquivo_hash, int tam)
{
    ListaCompartimentos *lcomp = cria_compartimentos(tam);
    for (int i = 0; i < tam; i++)
    {
        CompartimentoHash *comp = compartimento_hash(-1);
        lcomp->lista[i] = comp;
        free(comp);
    }
    salva_compartimentos(nome_arquivo_hash, lcomp);
}

int busca(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{

    ListaCompartimentos *lComp = le_compartimentos(nome_arquivo_hash);
    int tam = lComp->qtd;
    int h = hash(cod_cli, tam);
    int end = lComp->lista[h]->prox;
    free(lComp);
    return (end == -1) ? -1 : buscaRecursiva(end, cod_cli, nome_arquivo_dados);
}

int insere(int cod_cli, char *nome_cli, char *nome_arquivo_hash, char *nome_arquivo_dados, int num_registros)
{
    // TODO: Inserir aqui o codigo do algoritmo de insercao
    return INT_MAX;
}

int exclui(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
    FILE *arqDados = fopen(nome_arquivo_dados, "r+b");
    int end = busca(cod_cli, nome_arquivo_hash, nome_arquivo_dados);
    if (end == -1) return -1;
    fseek(arqDados, (end * sizeof(Cliente)), SEEK_SET);
    Cliente *cli = le_cliente(arqDados);
    cli->status = LIBERADO;
    fseek(arqDados, (end * sizeof(Cliente)), SEEK_SET);
    salva_cliente(cli, arqDados);
    free(cli);
    fclose(arqDados);

    return end;
}
