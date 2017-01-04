#ifndef ARTIGO_H
#define ARTIGO_H

typedef struct{
	int id;
	char titulo[300];
	unsigned short ano;
	char autores[1024];
	unsigned short citacoes;
	char atualizacao[19];
	char snippet[1024];
}Artigo;

#endif /*__ARTIGO_H__*/
