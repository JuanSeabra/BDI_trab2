#ifndef HASHING_H
#define HASHING_H

#include <stdio.h>
#include <string.h>
#include <algorithm>

#include "murmurhash.h"
#include "artigo.h"

/*Bucket escrito no arquivo*/
typedef struct {
	Artigo bloco[3]; //cluster de blocos
	int num_registros_ocupados;
	int ptr_arquivo_de_overflow;
} Bucket;

/*Item do arquivo de overflow*/
typedef struct {
	Artigo item;
	int ptr_prox;
} Overflow;

class HashBuckets {
public:
	//Construtor
	HashBuckets (FILE *hashFile, FILE *dataFile, FILE *overflowFile, int num_buckets);
	
	//Inserção
	//reorna 1 se houve overflow e 0 se não houve
	int insert(Artigo registro);

private:
	FILE *dataFile, *overflowFile;
	int *bucket_ptrs; //-1 = NULL, qualquer outro valor = endereço do bucket
	int num_buckets;


	Bucket* criaBucket();
	Overflow* criaOverflow(Artigo a);
	int HashString (char *s);
	void insereBucketEmArquivo(Bucket* bucket_em_memoria);
	void insereRegistroEmOverflow(Artigo registro);
	int insereRegistroEmBucket(Artigo registro, Bucket *bucket_em_memoria);
};

#endif 
