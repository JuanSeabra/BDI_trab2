#include "hashing.h"
#include <iostream>


//Construtor:
//entrada, arquivo de dados, nome do arquivo de overflow, numero de buckets e modo de abertura (w para escrita e r para leitura)
HashBuckets::HashBuckets(FILE *dataFile, char *overflowFileName, int num_bucketsTotais, char modo){
	if ( modo == 'w'){
		hashFile = fopen("bucketIndexes","w+");
		bucket_ptrs = new int[num_bucketsTotais];
		this->num_bucketsTotais = num_bucketsTotais;
		this->dataFile = dataFile;
		this->overflowFile = fopen(overflowFileName,"w+");
		this->num_buckets = 0;
		std::fill_n(bucket_ptrs, num_bucketsTotais, -1);
	}
	if (modo == 'r'){
		hashFile = fopen("bucketIndexes","r");
		bucket_ptrs = new int[num_bucketsTotais];
		fread(bucket_ptrs,sizeof(int),num_bucketsTotais,hashFile);
		this->num_bucketsTotais = num_bucketsTotais;
		this->dataFile = dataFile;
		this->overflowFile = fopen(overflowFileName,"r");
		fread(&num_buckets,sizeof(int),1,hashFile);
	}
}

//Destrutor
//escreve o arquivo de hash e fecha os arquivos
HashBuckets::~HashBuckets(){
	fseek(hashFile,0,SEEK_SET);
	fwrite(bucket_ptrs,sizeof(int),num_bucketsTotais,hashFile);
	fwrite(&num_buckets,sizeof(int),1,hashFile);
	fclose(hashFile);
	fclose(overflowFile);
	//delete bucket_ptrs;
}

//Função de hash retorna um inteiro hash do id
int HashBuckets::HashInt(int s){
	return s%num_bucketsTotais;
}

//Cria um bucket vazio, retorna um ponteiro para um bucket
Bucket* HashBuckets::criaBucket(){
	Bucket *item = new Bucket;
	item->num_registros_ocupados = 0;
	item->ptr_arquivo_de_overflow = -1;
	return item;
}

//cria um overflow vazio, retorna um ponteiro para um overflow
Overflow* HashBuckets::criaOverflow(){
	Overflow *item = new Overflow;
	item->num_registros_ocupados = 0;
	item->ptr_prox = -1;
	return item;
}

//inserção, retorna 1 se houve overflow e zero se não houve
//insere artigos no data file e no overflow
int HashBuckets::insert(Artigo registro){
	int bucket_index = HashInt(registro.id);
	int ptr_overflow_lido, ptr_overflow_anterior;
	Bucket *bucket_em_memoria;
	Overflow *overflow_em_memoria;

	//se o hash não aponta para nenhum bucket cria-se um novo
	if(bucket_ptrs[bucket_index] == -1){
		fseek(dataFile,0,SEEK_END);
		bucket_ptrs[bucket_index] = ftell(dataFile);
		bucket_em_memoria = criaBucket();
		bucket_em_memoria->bloco[0] = registro;
		bucket_em_memoria->num_registros_ocupados++;
		fwrite(bucket_em_memoria,sizeof(Bucket),1,dataFile);
		num_buckets++;
		delete bucket_em_memoria;
		return 0;
	}

	//Se aponta para um bucket ele é carregado para a memória
	bucket_em_memoria = new Bucket;
	fseek(dataFile,bucket_ptrs[bucket_index],SEEK_SET);
	fread(bucket_em_memoria,sizeof(Bucket),1,dataFile);

	//se há espaço o artigo é escrito no bucket
	if(bucket_em_memoria->num_registros_ocupados < 7){
		bucket_em_memoria->bloco[bucket_em_memoria->num_registros_ocupados] = registro;
		bucket_em_memoria->num_registros_ocupados++;
		fseek(dataFile,bucket_ptrs[bucket_index],SEEK_SET);
		fwrite(bucket_em_memoria,sizeof(Bucket),1,dataFile);
		num_buckets++;
		delete bucket_em_memoria;
		return 0;
	}

	//Se não há espaço e não há uma lista de overflow criada
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
		delete bucket_em_memoria;
		delete overflow_em_memoria;
		return 1;
	}
	//se não há espaço e há uma lista de overflow
	ptr_overflow_lido = bucket_em_memoria->ptr_arquivo_de_overflow;
	overflow_em_memoria =  new Overflow;

	//enquanto não for o último item da lista de overflow
	while (ptr_overflow_lido != -1) {
		fseek(overflowFile,ptr_overflow_lido,SEEK_SET);
		fread(overflow_em_memoria,sizeof(Overflow),1,overflowFile);

		//se há espaço no item da lista de overflow o artigo é inserido no item de overflow
		if(overflow_em_memoria->num_registros_ocupados < 7){
			overflow_em_memoria->bloco[overflow_em_memoria->num_registros_ocupados] = registro;
			overflow_em_memoria->num_registros_ocupados++;
			fwrite(overflow_em_memoria,sizeof(Overflow),1,overflowFile);
			num_buckets++;
			delete overflow_em_memoria;
			return 1;
		}
		ptr_overflow_anterior = ptr_overflow_lido;
		ptr_overflow_lido = overflow_em_memoria->ptr_prox;
	}
	//se chegou ao último elemento da lista e não há espaço no overflow cria-se um novo e insere o elemento
	fseek(overflowFile,ptr_overflow_anterior,SEEK_SET);
	fread(overflow_em_memoria,sizeof(Overflow),1,overflowFile);
	fseek(overflowFile,0,SEEK_END);
	overflow_em_memoria->ptr_prox = ftell(overflowFile);
	overflow_em_memoria = criaOverflow();
	overflow_em_memoria->bloco[0] = registro;
	overflow_em_memoria->num_registros_ocupados++;
	fwrite(overflow_em_memoria,sizeof(Overflow),1,overflowFile);
	num_buckets++;
	delete overflow_em_memoria;
	return 1;
}

//Busca, retorna um ponteiro para o item no arquivo de dados
Artigo* HashBuckets::search(int id){
	int bucket_index = HashInt(id);
	int ptr_overflow_lido;
	int num_blocos_lidos = 0;
	Bucket *bucket_em_memoria;
	Artigo *artigo_lido;
	Overflow *overflow_em_memoria;

	//Se o hash não aponta para nenhum bucket
	if (bucket_ptrs[bucket_index] == -1){
		std::cout << "Número de blocos lidos: " << num_blocos_lidos << std::endl;
		return NULL;
	}

	//Se o hash aponta para um bucket ele é carregado para a memória
	bucket_em_memoria = new Bucket;
	fseek(dataFile,bucket_ptrs[bucket_index],SEEK_SET);
	fread(bucket_em_memoria,sizeof(Bucket),1,dataFile);
	num_blocos_lidos++;

	//É feita a busca dentro do bucket se encontrado retorna
	for (int i = 0; i < bucket_em_memoria->num_registros_ocupados; ++i) {
		if(bucket_em_memoria->bloco[i].id == id){
			artigo_lido = new Artigo;
			*artigo_lido = bucket_em_memoria->bloco[i];
			std::cout << "Número de blocos lidos: " << num_blocos_lidos << std::endl;
			return artigo_lido;
		}
	}

	//Se o artigo não está no bucket e não existe lista de overflow, retorna NULL
	if(bucket_em_memoria->ptr_arquivo_de_overflow == -1){
		std::cout << "Número de blocos lidos: " << num_blocos_lidos << std::endl;
		return NULL;
	}

	//Se existe lista de overflow
	overflow_em_memoria = new Overflow;
	ptr_overflow_lido = bucket_em_memoria->ptr_arquivo_de_overflow;

	//Caminha na lista de overflow
	while (ptr_overflow_lido != -1) {
		fseek(overflowFile,ptr_overflow_lido,SEEK_SET);
		fread(overflow_em_memoria,sizeof(Overflow),1,overflowFile);
		num_blocos_lidos++;

		//Procura o elemento dentro do item do overflow
		for (int j = 0; j < overflow_em_memoria->num_registros_ocupados; ++j) {
			if (overflow_em_memoria->bloco[j].id == id){
			artigo_lido = new Artigo;
			*artigo_lido = overflow_em_memoria->bloco[j];
			std::cout << "Número de blocos lidos: " << num_blocos_lidos << std::endl;
			return artigo_lido;
			}
		}
		ptr_overflow_lido = overflow_em_memoria->ptr_prox;
	}

	//Se o id buscado não existe dentro do Overflow retorna NULL
	std::cout << "Número de blocos lidos: " << num_blocos_lidos << std::endl;
	return NULL;
}
