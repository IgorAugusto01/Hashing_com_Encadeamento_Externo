#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <limits.h>
#include <stdio.h>

#include "cliente.h"
#include "compartimento_hash.h"
#include "lista_compartimentos.h"
#include "encadeamento_exterior.h"

int metodo_divisao(int cod_cli, char* nome_arquivo_hash) {
	ListaCompartimentos* listaHash = le_compartimentos(nome_arquivo_hash);
	return (cod_cli % listaHash->qtd);
}

void cria_hash(char *nome_arquivo_hash, int tam)
{
	FILE* arq;

	if((arq = fopen(nome_arquivo_hash, "wb")) != NULL) {
		int v[tam];

		for(int i = 0; i < tam; i++) 
			v[i] = -1;

		fwrite(v, sizeof(int), tam, arq);
		fclose(arq);
	} else {
		printf("Não foi possivel criar o arquivo Tabela Hash!\n");
	}
}

int busca(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)
{
	FILE* hash;
    FILE* dados;

	int hX = metodo_divisao(cod_cli, nome_arquivo_hash);

    // Abre os arquivos de hash e dados
    if((hash = fopen(nome_arquivo_hash, "rb")) != NULL && (dados = fopen(nome_arquivo_dados, "rb")) != NULL) {

		fseek(hash, hX * sizeof(int), SEEK_SET);

		CompartimentoHash* compartimento = le_compartimento(hash);

		if(compartimento->prox != -1) {

			fseek(dados, compartimento->prox * sizeof(Cliente), SEEK_SET);

			Cliente* clienteBusca = le_cliente(dados);

			while((clienteBusca->cod_cliente != cod_cli || clienteBusca->status == 0) && clienteBusca->prox != -1) {
				compartimento->prox = clienteBusca->prox;

				fseek(dados, clienteBusca->prox * sizeof(Cliente), SEEK_SET);
				clienteBusca = le_cliente(dados);
			}

			if(cod_cli == clienteBusca->cod_cliente && clienteBusca->status == 1) {
				fclose(hash);
				fclose(dados);
				free(clienteBusca);
				return compartimento->prox;
			}

		}

		fclose(hash);
		fclose(dados);

		return -1;	//  compartimento vazio, ou não achou registro (apagado ou inexistente).
	}
}

int insere(int cod_cli, char *nome_cli, char *nome_arquivo_hash, char *nome_arquivo_dados, int num_registros)
{
	FILE* hash;
	FILE* dados;

	CompartimentoHash* compartimentoHash;

	int hX = metodo_divisao(cod_cli, nome_arquivo_hash);

	if((hash = fopen(nome_arquivo_hash, "rb+")) != NULL) {

		fseek(hash, hX * sizeof(int), SEEK_SET);

		compartimentoHash = le_compartimento(hash);

		if(compartimentoHash->prox == -1) {	// Compartimento da tabela Hash vazio (-1)

			if((dados = fopen(nome_arquivo_dados, "rb+")) != NULL) {

				Cliente* clienteEscrita = cliente(cod_cli, nome_cli, -1, 1);

				fseek(dados, num_registros * sizeof(Cliente), SEEK_SET);
				salva_cliente(clienteEscrita, dados);

				fseek(hash, hX * sizeof(int), SEEK_SET);
				salva_compartimento(compartimento_hash(num_registros), hash);

				free(clienteEscrita);

				fclose(hash);
				fclose(dados);

				return num_registros;

			}

		}

		fclose(hash);

	}

	int posicaoBusca = busca(cod_cli, nome_arquivo_hash, nome_arquivo_dados);

	if(posicaoBusca != -1) {	// Chave escolhida JÁ existente no arquivo de dados

		return -1;

	} else {	// Chave escolhida NÂO existente no arquivo de dados

		if((hash = fopen(nome_arquivo_hash, "rb")) != NULL && (dados = fopen(nome_arquivo_dados, "rb+")) != NULL) {

			int posicaoLiberado = -1;

			fseek(hash, hX * sizeof(int), SEEK_SET);
			compartimentoHash = le_compartimento(hash);

			fseek(dados, compartimentoHash->prox * sizeof(Cliente), SEEK_SET);
			Cliente* clienteAnterior = le_cliente(dados);

			while((clienteAnterior->cod_cliente != cod_cli || clienteAnterior->status == 0) && clienteAnterior->prox != -1) {	// Laço para encontrar espaço para o novo registro
				
				if(clienteAnterior->status == 0) {	// Registro LIBERADO é guardado para ser substituido pela nova chave
					posicaoLiberado = compartimentoHash->prox;
				}

				compartimentoHash->prox = clienteAnterior->prox;

				fseek(dados, clienteAnterior->prox * sizeof(Cliente), SEEK_SET);
				clienteAnterior = le_cliente(dados);
			}

			if(posicaoLiberado == -1) {	// Inserido no final do arquivo de dados

				clienteAnterior->prox = num_registros;
	
				fseek(dados, compartimentoHash->prox * sizeof(Cliente), SEEK_SET);
				salva_cliente(clienteAnterior, dados);
	
				Cliente* clienteEscrita = cliente(cod_cli, nome_cli, -1, 1);
	
				fseek(dados, 0, SEEK_END);
				salva_cliente(clienteEscrita, dados);
	
				free(clienteEscrita);

				fclose(hash);
				fclose(dados);
	
				return num_registros;

			} else {	// Inserido em um registro LIBERADO

				fseek(dados, compartimentoHash->prox * sizeof(Cliente), SEEK_SET);
				salva_cliente(clienteAnterior, dados);

				Cliente* clienteEscrita = cliente(cod_cli, nome_cli, compartimentoHash->prox, 1);

				fseek(dados, posicaoLiberado * sizeof(Cliente), SEEK_SET);
				salva_cliente(clienteEscrita, dados);

				free(clienteEscrita);
				
				fclose(hash);
				fclose(dados);

				return posicaoLiberado;

			}
		}
	}
}

int exclui(int cod_cli, char *nome_arquivo_hash, char *nome_arquivo_dados)	// Feito por Igor
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
