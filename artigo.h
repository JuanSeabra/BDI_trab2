#ifndef ARTIGO_H
#define ARTIGO_H

typedef struct{
	int id;
	char titulo[301];
	unsigned short ano;
	char autores[101];
	unsigned citacoes;
	char atualizacao[20];
	char snippet[101];
}Artigo;

#endif /*__ARTIGO_H__*/
