#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <boost/tokenizer.hpp>
using boost::tokenizer;
using boost::escaped_list_separator;
using boost::split;
using boost::is_any_of;
using boost::erase_all;
using namespace std;

#include "artigo.h"
#include "hashing.h"
#include "btree_prim.h"
#include "btree_sec.h"
#include "auxiliar.h"


//	   hashing file <waldomiro>
//     primary index file (B-Tree) <victoria>
//     secondary index file (B-Tree) <victoria e waldomiro>

/*
	Funcao que recebe uma string e retorna uma lista de strings obtidas
	a partir do parser da string de entrada, ou seja, separando os dados 
	atraves	do separador
*/
vector<string> parse(string s){
	erase_all(s,"\\");
	typedef tokenizer<escaped_list_separator<char> > so_tokenizer;
	vector<string> strs;
	so_tokenizer tok(s, escaped_list_separator<char>('\\', ';', '\"'));
	for(so_tokenizer::iterator beg=tok.begin(); beg!=tok.end(); ++beg){
		strs.push_back(*beg);
	}
	return strs;
}

//Cria o arquivo de indice primario com arvore B
void criaIndicePrimario(HashBuckets& hash_registros, BTTablePrimClass& btree) {
	TipoIndicePrim itemIndice;

	for (int i = 0; i < NUM_BUCKETS; i++) {
		itemIndice.id = i;
		itemIndice.pontBucket = hash_registros.bucket_ptrs[i];

		btree.inserir(itemIndice);
	}
}

//Cria o arquivo de indice secundario com arvore B e o arquivo de blocos auxiliares
void criaIndiceSecundario(HashBuckets& hash_registros, BTTableSecClass& btree ) {
	TipoIndiceSec itemSec;
	int bucket_endereco;
	Bucket *bucket_em_memoria = new Bucket;
	Artigo art;
	Auxiliar blocosAuxiliares ('w', "blocoAux");

	//percorre o hash sequencialmente
	//para cada bucket
	for (int i = 0; i < NUM_BUCKETS; i++) {
		bucket_endereco = hash_registros.bucket_ptrs[i];
		//verifica se o bucket esta realmente ocupado
		if (bucket_endereco != -1) {
			fseek(hash_registros.dataFile,bucket_endereco,SEEK_SET);
			fread(bucket_em_memoria,sizeof(Bucket),1,hash_registros.dataFile);

			//para cada artigo no bucket
			for(int j = 0; j < bucket_em_memoria->num_registros_ocupados; j++) {
				art = bucket_em_memoria->bloco[j];
				BlocoAuxiliar blocoAux;

				/*se ja existe este titulo na btree, acrescenta uma ocorrencia
				  no bloco auxiliar
				*/
				if (btree.recuperar(art.titulo, itemSec)) {
					unsigned endBloco = itemSec.pontBloco;
					blocoAux = blocosAuxiliares.recuperar(endBloco);
					int pos = blocoAux.cont;

					blocoAux.pontBucket[pos] = bucket_endereco;
					blocoAux.posBucket[pos] = j;
					blocosAuxiliares.atualizar(endBloco, blocoAux);
				}
				//senao, cria um bloco auxiliar com a ocorrencia e coloca na btree
				else {
					blocoAux.pontBucket[0] = bucket_endereco;
					blocoAux.posBucket[0] = j;
					blocoAux.cont = 1;
					unsigned endBloco = blocosAuxiliares.inserir(blocoAux);

					strcpy(itemSec.titulo, art.titulo);
					itemSec.pontBloco = endBloco;
					btree.inserir(itemSec);
				}
			}
		}

	}

	delete bucket_em_memoria;
}


int main(int argc, char *argv[]){
	ifstream f;
	FILE *out;
	string linha,linha_aux;
	Artigo registro;
	int count = 0;
	vector<string> strs;
	BTTablePrimClass btreePrim('w', "indicePrimario.dat");
	BTTableSecClass btreeS('w', "indiceSecundario.dat");


	if(argc != 2){
		cout << "Modo de uso ./upload <arquivo de entrada>" << endl;
		exit(0);
	}

	//arquivo organizado por hashing
	f.open(argv[1]);
	out = fopen("Artigo.dat","w+");
	HashBuckets hash_registros (out,"overflowFile", NUM_BUCKETS, 'w');

	cout << "Organizando o arquivo de dados por hashing... aguarde" << endl;

	//realizando o parsing da massa de entrada
	while (getline(f,linha)) {
		strs = parse(linha);

		//Correção de erro de quebra de linha
		if (strs.size() < 7){
			getline(f,linha_aux);
			linha += linha_aux;
			strs = parse(linha);
		}

		registro.id = stoi(strs[0]);


		strncpy(registro.titulo,strs[1].c_str(),300);
		registro.ano = (unsigned short) stoi(strs[2]);
		strncpy(registro.autores,strs[3].c_str(),100);
		registro.citacoes = (unsigned) stoi(strs[4]);
		strncpy(registro.atualizacao,strs[5].c_str(),19);
		strncpy(registro.snippet,strs[6].c_str(),100);
		count++;

		strs.clear();
		hash_registros.insert(registro);
	}

	f.close();

	cout << "Foram escritos " << count << " registros!" << endl;

	cout << "Arquivo organizado por hashing criado!" << endl;

	cout << "Criando indice primario, por favor aguarde" << endl;
	criaIndicePrimario(hash_registros, btreePrim);
	cout << "Indice primario criado!" << endl;

	cout << "Criando indice secundario, por favor aguarde" << endl;
	criaIndiceSecundario(hash_registros, btreeS);
	cout << "Indice secundario criado!" << endl;

	fclose(out);

	return 0;
}
