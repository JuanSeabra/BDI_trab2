#include "hashing.h"
#include <iostream>

int main(int argc, char *argv[]){
	if (argc != 2) {
		exit (0);
	}
	FILE *dataFile;
	Artigo *artigo_buscado = NULL;
	dataFile = fopen("Artigo.dat","rb");
	HashBuckets hash_registros (dataFile,"overflowFile", NUM_BUCKETS, 'r');
	artigo_buscado = hash_registros.search(atoi(argv[1]));
	std::cout << "Total de blocos do arquivo de dados: " << hash_registros.num_buckets << std::endl;
	if(artigo_buscado == NULL){
		std::cout << "id buscado não existe!" << std::endl;
		return 0;
	}
	std::cout << "ID: " << artigo_buscado->id << std::endl
		<< " Titulo: " << artigo_buscado->titulo << std::endl
		<< " Autores: " << artigo_buscado->autores << std::endl
		<< " Ano: " << artigo_buscado->ano << std::endl
		<< " Citações: " << artigo_buscado->citacoes << std::endl
		<< " Atualização: " << artigo_buscado->atualizacao << std::endl
		<< " Snippet: " << artigo_buscado->snippet << std::endl;

	return 0;
}
