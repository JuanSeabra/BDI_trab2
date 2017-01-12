#include "hashing.h"
#include <iostream>

HashBuckets::HashBuckets(char *hashFileName, FILE *dataFile, char *overflowFileName, int num_bucketsTotais){
	hashFile = fopen(hashFileName,"r");
	fread(bucket_ptrs,sizeof(int),num_bucketsTotais,hashFile);
	this->num_bucketsTotais = num_bucketsTotais;
	this->dataFile = dataFile;
	this->overflowFile = fopen(overflowFileName,"r");
	this->num_buckets = 0;
	for (int i = 0; i < num_bucketsTotais; ++i) {
		std::cout << "|" << bucket_ptrs[i] <<"|";
	}

}

HashBuckets::HashBuckets(FILE *dataFile, char *overflowFileName, int num_bucketsTotais){
	hashFile = fopen("bucketIndexes","w+");
	bucket_ptrs = new int[num_bucketsTotais];
	this->num_bucketsTotais = num_bucketsTotais;
	this->dataFile = dataFile;
	this->overflowFile = fopen(overflowFileName,"w+");
	this->num_buckets = 0;
	std::fill_n(bucket_ptrs, num_bucketsTotais, -1);
}

HashBuckets::~HashBuckets(){
	fseek(hashFile,0,SEEK_SET);
	fwrite(bucket_ptrs,sizeof(int),num_bucketsTotais,hashFile);
	fclose(hashFile);
	fclose(overflowFile);
	for (int i = 0; i < num_bucketsTotais; ++i) {
		std::cout << "|" << bucket_ptrs[i] <<"|";
	}
	//delete bucket_ptrs;
}

int HashBuckets::HashInt(int s){
	return s%num_bucketsTotais;
}

Bucket* HashBuckets::criaBucket(){
	Bucket *item = new Bucket;
	item->num_registros_ocupados = 0;
	item->ptr_arquivo_de_overflow = -1;
	return item;
}

Overflow* HashBuckets::criaOverflow(){
	Overflow *item = new Overflow;
	item->num_registros_ocupados = 0;
	item->ptr_prox = -1;
	return item;
}

Artigo* HashBuckets::buscaNoBucket(int id, Bucket *bucket_em_memoria){
	for (int i = 0; i < bucket_em_memoria->num_registros_ocupados; ++i) {
		if (bucket_em_memoria->bloco[i].id == id) return &(bucket_em_memoria->bloco[i]) ;
	}
	return NULL;
}

int HashBuckets::insert(Artigo registro){
	int bucket_index = HashInt(registro.id);
	int ptr_overflow_lido, ptr_overflow_anterior;
	Bucket *bucket_em_memoria;
	Overflow *overflow_em_memoria;

	if(bucket_ptrs[bucket_index] == -1){
		fseek(dataFile,0,SEEK_END);
		bucket_ptrs[bucket_index] = ftell(dataFile);
		bucket_em_memoria = criaBucket();
		bucket_em_memoria->bloco[0] = registro;
		bucket_em_memoria->num_registros_ocupados++;
		fwrite(bucket_em_memoria,sizeof(Bucket),1,dataFile);
		num_buckets++;
		//std::cout << "teste[1]" << std::endl;
		delete bucket_em_memoria;
		return 0;
	}
	bucket_em_memoria = new Bucket;
	fseek(dataFile,bucket_ptrs[bucket_index],SEEK_SET);
	//std::cout << ftell(dataFile) << std::endl;
	fread(bucket_em_memoria,sizeof(Bucket),1,dataFile);
	if(bucket_em_memoria->num_registros_ocupados < 7){
		bucket_em_memoria->bloco[bucket_em_memoria->num_registros_ocupados] = registro;
		bucket_em_memoria->num_registros_ocupados++;
		fseek(dataFile,bucket_ptrs[bucket_index],SEEK_SET);
		fwrite(bucket_em_memoria,sizeof(Bucket),1,dataFile);
		num_buckets++;
		//std::cout << "teste[2]" << std::endl;
		delete bucket_em_memoria;
		return 0;
	}
	if(bucket_em_memoria->ptr_arquivo_de_overflow == -1){
		fseek(overflowFile,0,SEEK_END);
		bucket_em_memoria->ptr_arquivo_de_overflow = ftell(overflowFile);
		fseek(dataFile,bucket_ptrs[bucket_index],SEEK_SET);
		fwrite(bucket_em_memoria,sizeof(Bucket),1,dataFile);
		overflow_em_memoria = criaOverflow();
		overflow_em_memoria->bloco[0] = registro;
		overflow_em_memoria->num_registros_ocupados++;
		fwrite(overflow_em_memoria,sizeof(Overflow),1,overflowFile);
		num_buckets++;
		//std::cout << "teste[3]" << std::endl;
		delete bucket_em_memoria;
		delete overflow_em_memoria;
		return 1;
	}
	ptr_overflow_lido = bucket_em_memoria->ptr_arquivo_de_overflow;
	overflow_em_memoria =  new Overflow;
	while (ptr_overflow_lido != -1) {
		fseek(overflowFile,ptr_overflow_lido,SEEK_SET);
		fread(overflow_em_memoria,sizeof(Overflow),1,overflowFile);
		if(overflow_em_memoria->num_registros_ocupados < 7){
			overflow_em_memoria->bloco[overflow_em_memoria->num_registros_ocupados] = registro;
			overflow_em_memoria->num_registros_ocupados++;
			fwrite(overflow_em_memoria,sizeof(Overflow),1,overflowFile);
			num_buckets++;
			//std::cout << "teste[4]" << std::endl;
			delete overflow_em_memoria;
			return 1;
		}
		ptr_overflow_anterior = ptr_overflow_lido;
		ptr_overflow_lido = overflow_em_memoria->ptr_prox;
		//delete overflow_em_memoria;
	}
	fseek(overflowFile,ptr_overflow_anterior,SEEK_SET);
	fread(overflow_em_memoria,sizeof(Overflow),1,overflowFile);
	fseek(overflowFile,0,SEEK_END);
	overflow_em_memoria->ptr_prox = ftell(overflowFile);
	overflow_em_memoria = criaOverflow();
	overflow_em_memoria->bloco[0] = registro;
	overflow_em_memoria->num_registros_ocupados++;
	fwrite(overflow_em_memoria,sizeof(Overflow),1,overflowFile);
	num_buckets++;
	//std::cout << "teste[5]" << std::endl;
	delete overflow_em_memoria;
	return 1;
}

Artigo*	HashBuckets::search(int id){
	Bucket *bucket_em_memoria =  new Bucket;
	Artigo *artigo_retornado = new Artigo;
	int index = HashInt(id);
	int indexB = bucket_ptrs[index];
	if (indexB == -1) return NULL;
	fseek(dataFile,indexB,SEEK_SET);
	fread(bucket_em_memoria,sizeof(Bucket),1,dataFile);
	artigo_retornado = buscaNoBucket(id,bucket_em_memoria);
	return artigo_retornado;
}
