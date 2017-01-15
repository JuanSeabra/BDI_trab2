#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>

using namespace std;

#include "artigo.h"
#include "hashing.h"
#include "btree_prim.h"
#include "btree_sec.h"
#include "auxiliar.h"

#define NUM_BUCKETS 218880

void imprimeArtigo(Artigo art) {
	cout << "ID: " << art.id << endl;
	cout << "Titulo: " << art.titulo << endl;
	cout << "Ano: " << art.ano << endl;
	cout << "Autores: " << art.autores << endl;
	cout << "Citacoes: " << art.citacoes << endl;
	cout << "Atualizacao: " << art.atualizacao << endl;
	cout << "Snippet: " << art.snippet << endl;
	cout << endl;
}

int main(int argc, char *argv[]) {
	char *titulo_buscado;
	int blocosLidos;
	TipoIndiceSec itemBuscado;
	BTTableSecClass btree_sec('r', "indiceSecundario.dat");
	FILE *arquivo_dados, *arquivo_hash;
	Auxiliar blocosAuxiliares('r', "blocoAux");

	if(argc != 2){
		cout << "Modo de uso ./seek2 <titulo>" << endl;
		exit(0);
	}

	arquivo_dados = fopen("Artigo.dat","rb");
	HashBuckets hash_registros (arquivo_dados,"overflowFile", NUM_BUCKETS, 'r');
	titulo_buscado = argv[1];

	if (btree_sec.recuperarContBlocos(titulo_buscado, itemBuscado, blocosLidos)) {
		//obter o endereco do bloco auxiliar contendo estas ocorrencias
		int end_bloco = itemBuscado.pontBloco;
		BlocoAuxiliar blocoAux = blocosAuxiliares.recuperar(end_bloco);
		Artigo artigoBuscado;
		Bucket *bucket_em_memoria = new Bucket;

		for (int i = 0; i < blocoAux.cont; i++) {
			int bucket_endereco = blocoAux.pontBucket[i];
			int pos_bucket = blocoAux.posBucket[i];

			fseek(hash_registros.dataFile,bucket_endereco,SEEK_SET);
			fread(bucket_em_memoria,sizeof(Bucket),1,hash_registros.dataFile);

			artigoBuscado = bucket_em_memoria->bloco[pos_bucket];
			imprimeArtigo(artigoBuscado);
		}

		delete bucket_em_memoria;

		//imprimir o num de blocos lidos (num na arvore B + o bloco do bucket) e o num de blocos totais
		//cout << "Quantidade de ocorrencias: " << blocoAux.cont << endl;
		cout << "Blocos lidos para encontrar: " << blocosLidos << " (indice) + " << 1+ blocoAux.cont 
		<< " (bloco auxiliar com ponteiros + arquivo de dados)" << endl;
		cout << "Total de blocos indice secundario: " << btree_sec.numNos << endl;		
	}
	else {
		cout << "Nao existe arquivo com este titulo cadastrado" << endl;
	}

	return 0;
}