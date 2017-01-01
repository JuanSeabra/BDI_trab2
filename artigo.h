#ifndef ARTIGO_H
#define ARTIGO_H

typedef struct{
	int id;
	char titulo[250];
	unsigned short ano;
	char autores[50];
	unsigned short citacoes;
	char atualizacao[20];
	char snippet[1024];
}Artigo;

#endif
