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
	int id_buscado, hash_id_buscado;
	int blocosLidos;
	TipoIndicePrim itemBuscado;
	BTTablePrimClass btree_prim('r', "indicePrimario.dat");
	FILE *arquivo_dados, *arquivo_hash;

	if(argc != 2){
		cout << "Modo de uso ./seek1 <id>" << endl;
		exit(0);
	}

	arquivo_dados = fopen("Artigo.dat","rb");
	HashBuckets hash_registros (arquivo_dados,"overflowFile", NUM_BUCKETS, 'r');
	id_buscado = stoi(argv[1]);
	hash_id_buscado = hash_registros.HashInt(id_buscado);

	if (btree_prim.recuperarContBlocos(hash_id_buscado, itemBuscado, blocosLidos)) {
		//buscar no hash o artigo correspondente
		int bucket_endereco = itemBuscado.pontBucket;
		Bucket *bucket_em_memoria = new Bucket;
		Artigo artigoBuscado;
		bool achou = false;

		fseek(hash_registros.dataFile,bucket_endereco,SEEK_SET);
		fread(bucket_em_memoria,sizeof(Bucket),1,hash_registros.dataFile);

		int i = 0;
		while((i < bucket_em_memoria->num_registros_ocupados) && (!achou)) {
			if (bucket_em_memoria->bloco[i].id == id_buscado) {
				artigoBuscado = bucket_em_memoria->bloco[i];
				achou = true;
			}
			i++;
		}

		if (!achou) {
			cout << "Nao existe arquivo com este ID cadastrado" << endl;
		}
		else {
			//imprimir todos os campos do artigo
			imprimeArtigo(artigoBuscado);

			//imprimir o num de blocos lidos (num na arvore B + o bloco do bucket) e o num de blocos totais
			cout << "Blocos lidos para encontrar: " << blocosLidos + 1<< endl;
			cout << "Total de blocos indice primario: " << btree_prim.numNos << endl;
		}
	}
	else {
		cout << "Nao existe arquivo com este ID cadastrado" << endl;
	}

	return 0;
}