#ifndef HASHING_H
#define HASHING_H

#include <stdio.h>
#include <string.h>
#include <algorithm>

#include "artigo.h"

/*Bucket escrito no arquivo*/
typedef struct {
	Artigo bloco[7]; //blocos
	int num_registros_ocupados;
	int ptr_arquivo_de_overflow;
} Bucket;

/*Item do arquivo de overflow*/
typedef struct {
	Artigo bloco[7];
	int num_registros_ocupados;
	int ptr_prox;
} Overflow;

class HashBuckets {
	public:
		//Construtor
		HashBuckets (char *hashFileName, FILE *dataFile, char *overflowFileName, int num_buckets);
		HashBuckets(FILE *dataFile, char *overflowFileName, int num_buckets, char *hashFileName, char modo);
		~HashBuckets();

		//Inserção
		//reorna 1 se houve overflow e 0 se não houve
		int insert(Artigo registro);

		FILE *dataFile, *overflowFile, *hashFile;
		int *bucket_ptrs; //-1 = NULL, qualquer outro valor = endereço do bucket
		int num_buckets;
		int num_bucketsTotais;

		Bucket* criaBucket();
		Overflow* criaOverflow();
		int HashInt (int s);
};

#endif
