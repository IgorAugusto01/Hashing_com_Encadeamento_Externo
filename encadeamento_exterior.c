#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limits.h>
#include "lista_compartimentos.h"
#include "lista_clientes.h"
#include <stdlib.h>


int buscaRecuriva


int hash(int valor, int tam)
{
    return valor % tam;
}

ListaCompartimentos* cria_lista_compartimentos_vazia(int tam)
{
    ListaCompartimentos *lcompVazia = (ListaCompartimentos*)malloc(sizeof(ListaCompartimentos));
    lcompVazia->qtd = tam;
    lcompVazia->lista=(CompartimentoHash**)malloc(tam * sizeof(CompartimentoHash*));
    for(int i=0; i<tam; i++) lcompVazia->lista[i] = compartimento_hash(-1);
    return lcompVazia;

}

void cria_hash(char *nome_arquivo_hash, int tam)
{
    ListaCompartimentos *lcomp = cria_lista_compartimentos_vazia(tam);
    salva_compartimentos(nome_arquivo_hash,lcomp);



}

int busca(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
    ListaCompartimentos *lcomp = le_compartimentos(nome_arquivo_hash);
    ListaClientes *lcli = le_clientes(nome_arquivo_dados);

    int h = hash(cod_cli,lcomp->qtd);

    if(lcomp->lista[h]->prox == -1) return -1;
    if(lcli->lista[lcomp->lista[h]->prox]->cod_cliente == cod_cli) return 0;


    return INT_MAX;
}


int insere(int cod_cli, char *nome_cli, char *nome_arquivo_hash, char *nome_arquivo_dados, int num_registros)
{
    //TODO: Inserir aqui o codigo do algoritmo de insercao
    return INT_MAX;
}

int exclui(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
    //TODO: Inserir aqui o codigo do algoritmo de remocao
    return INT_MAX;
}
