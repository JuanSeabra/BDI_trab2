#include <iostream>
#include "artigo.h"
#include "hashing.h"

void imprimeArtigo(Artigo art){
	std::cout << "ID: " << art.id <<
		" Titulo: " << art.titulo <<
		" Autores: " << art.autores <<
		" Ano: " << art.ano <<
		" Atualização: " << art.atualizacao <<
		" Citaçoes: " << art.citacoes <<
		" Snippet :" << art.snippet << std::endl;
}
int main(int argc, char *argv[]){
	FILE *f;
	if (argc != 2) exit(0) ;
	f = fopen("Artigo.dat","r");
	HashBuckets hash ("bucketIndexes", f, "overflowFile", 218880);
	
	imprimeArtigo(*(hash.search(atoi(argv[1]))));
	return 0;
}
