#ifndef ARTIGO_H
#define ARTIGO_H

typedef struct{
	int id;
	char titulo[300];
	unsigned short ano;
	char autores[100];
	unsigned citacoes;
	char atualizacao[19];
	char snippet[100];
}Artigo;

#endif /*__ARTIGO_H__*/
