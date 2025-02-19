#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limits.h>
#include "lista_compartimentos.h"
#include "lista_clientes.h"
#include <stdlib.h>




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



int busca(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{



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
