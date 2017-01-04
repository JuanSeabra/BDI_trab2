#include "hashing.h"

HashBuckets::HashBuckets(FILE *hashFile, FILE *dataFile, FILE *overflowFile, int num_buckets){
	bucket_ptrs = new int[num_buckets];
	fread(bucket_ptrs,sizeof(int),sizeof(int)*num_buckets,hashFile);
	this->dataFile = dataFile;
	this->overflowFile = overflowFile;
	this->num_buckets = num_buckets;
	std::fill_n(bucket_ptrs, num_buckets, -1);
}

int HashBuckets::HashString(char *s){
	return murmurhash (s, strlen(s),0)%num_buckets;
}

Bucket* HashBuckets::criaBucket(){
	Bucket *item = new Bucket;
	item->num_registros_ocupados = 0;
	item->ptr_arquivo_de_overflow = -1;
	return item;
}

Overflow* HashBuckets::criaOverflow(Artigo a){
	Overflow* o = new Overflow;
	o->item = a;
	o->ptr_prox = -1;
	return o;
}

void HashBuckets::insereBucketEmArquivo(Bucket* bucket_em_memoria){
	fseek(dataFile,0,SEEK_END);
	fwrite(bucket_em_memoria,sizeof(Bucket),sizeof(Bucket),dataFile);
}

void HashBuckets::insereRegistroEmOverflow(Artigo registro){
	Overflow *registro_overflow = criaOverflow(registro);
	fseek(overflowFile,0,SEEK_END);
	fwrite(registro_overflow,sizeof(Overflow),sizeof(Overflow),overflowFile);
	delete registro_overflow;
}

int HashBuckets::insereRegistroEmBucket(Artigo registro, Bucket *bucket_em_memoria){
	Overflow *registro_overflow;
	int ptr_lista_overflow, ptr_anterior;
	if(bucket_em_memoria->num_registros_ocupados < 3){
		bucket_em_memoria->bloco[bucket_em_memoria->num_registros_ocupados] = registro;
		bucket_em_memoria->num_registros_ocupados++;
		return 0;
	}
	if(bucket_em_memoria->ptr_arquivo_de_overflow != -1){
		bucket_em_memoria->ptr_arquivo_de_overflow = ftell(overflowFile);
		insereRegistroEmOverflow(registro);
	}
	ptr_lista_overflow = bucket_em_memoria->ptr_arquivo_de_overflow;
	while(ptr_lista_overflow != -1){
		fread(registro_overflow,sizeof(Overflow),sizeof(Overflow),overflowFile);
		ptr_anterior = ptr_lista_overflow;
		ptr_lista_overflow = registro_overflow->ptr_prox;
	}

	fseek(overflowFile,0,SEEK_END);
	registro_overflow->ptr_prox = ftell(overflowFile);
	fseek(overflowFile,ptr_anterior,SEEK_SET);
	fwrite(registro_overflow,sizeof(Overflow),sizeof(Overflow),overflowFile);
	insereRegistroEmOverflow(registro);
	return 1;
}

int HashBuckets::insert(Artigo registro){
	int bucket_index = HashString(registro.titulo);
	Bucket *bucket_em_memoria;
	int status_overflow = 0;

	//se local está vazio
	if (bucket_ptrs[bucket_index] == -1) {
		bucket_em_memoria = criaBucket();
		bucket_ptrs[bucket_index] = ftell(dataFile);
	} else {
		fseek(dataFile,bucket_ptrs[bucket_index],SEEK_SET);
		fread(bucket_em_memoria,sizeof(Bucket),sizeof(Bucket),dataFile);
	}
	status_overflow = insereRegistroEmBucket(registro, bucket_em_memoria);
	insereBucketEmArquivo(bucket_em_memoria);
	delete bucket_em_memoria;
	return status_overflow;
}
