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

int posicaoLivre(char *nome_arquivo_dados)
{
    FILE *arqDados = fopen(nome_arquivo_dados, "rb");
    int i = 0;
    while (1)
    {
        fseek(arqDados, (i * sizeof(Cliente)), SEEK_SET);
        Cliente *c = le_cliente(arqDados);
        if (c == NULL)
        {
            free(c);
            break;
        }
        i++;
    }
    fclose(arqDados);
    return i;
}

int hash(int valor, int tam)
{
    return valor % tam;
}

void cria_hash(char *nome_arquivo_hash, int tam)
{
    FILE* tab_hash = fopen(nome_arquivo_hash,"wb");
   int i = 0;
    while(i < tam)
    {
     fseek(tab_hash,(i*sizeof(CompartimentoHash)),SEEK_SET);
     CompartimentoHash *comp = compartimento_hash(-1);
     salva_compartimento(comp,tab_hash);
     free(comp);
     i++;
    }
    fclose(tab_hash);
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
    FILE *arqDados = fopen(nome_arquivo_dados, "rb+");

    int end = busca(cod_cli, nome_arquivo_hash, nome_arquivo_dados);
    if (end != -1)
    {
        fseek(arqDados, (end * sizeof(Cliente)), SEEK_SET);
        Cliente *c = le_cliente(arqDados);
        if (c->status == OCUPADO)
        {
            free(c);
            fclose(arqDados);
            return -1;
        }
        else
        {
            c->status = OCUPADO;
            fseek(arqDados, (end * sizeof(Cliente)), SEEK_SET);
            salva_cliente(c, arqDados);
            free(c);
            fclose(arqDados);
            return end;
        }
    }

    FILE *tab_hash = fopen(nome_arquivo_hash, "rb+");
    ListaCompartimentos *lc = le_compartimentos(nome_arquivo_hash);
    int pLivre = posicaoLivre(nome_arquivo_dados);
    int h = hash(cod_cli, lc->qtd);
    free(lc);
    fseek(tab_hash, (h * sizeof(CompartimentoHash)), SEEK_SET);
    CompartimentoHash *comp = le_compartimento(tab_hash);
    if (comp->prox == -1)
    {
        comp->prox = pLivre;
        fseek(tab_hash, (h * sizeof(CompartimentoHash)), SEEK_SET);
        salva_compartimento(comp, tab_hash);
        free(comp);
        fseek(arqDados, (pLivre * sizeof(Cliente)), SEEK_SET);
        Cliente *c = cliente(cod_cli, nome_cli, -1, OCUPADO);
        salva_cliente(c, arqDados);
        free(c);
        fclose(arqDados);
        fclose(tab_hash);
        return pLivre;
    }

     end = comp->prox;
    fclose(tab_hash);
    free(comp);
    while (1)
    {
        fseek(arqDados, (end * sizeof(Cliente)), SEEK_SET);
        Cliente *c = le_cliente(arqDados);
        if (c->status == LIBERADO)
        {
            int prox = c->prox;
            free(c);
            c = cliente(cod_cli, nome_cli, prox, OCUPADO);
            fseek(arqDados, (end * sizeof(Cliente)), SEEK_SET);
            salva_cliente(c, arqDados);
            free(c);
            fclose(arqDados);
            return end;
        }
        else if (c->prox == -1)
        {
            c->prox = pLivre;
            fseek(arqDados, (end * sizeof(Cliente)), SEEK_SET);
            salva_cliente(c, arqDados);
            free(c);
            c = cliente(cod_cli, nome_cli, -1, OCUPADO);
            fseek(arqDados, (pLivre * sizeof(Cliente)), SEEK_SET);
            salva_cliente(c, arqDados);
            free(c);
            fclose(arqDados);
            return pLivre;
        }
        end = c->prox;
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
