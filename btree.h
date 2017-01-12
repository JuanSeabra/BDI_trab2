#include "tabela.h"

const int mP = 170; //ordem m da arvore B
const int maxChaves = 2*mP; //numero maximo de chaves
const int minChaves = mP;   //numero minimo de chaves
const int pontNull = -1;  //ponteiro para null


typedef struct {
	int qtd; //numero de chaves/pont atualmente no no
	TipoIndicePrim chave[maxChaves]; //chaves/ponteiros
	int ponteiros[maxChaves+1]; //apontadores da arvore B
}TipoNoPrim;

class BTTablePrimClass : public TabelaBasePrimClass {
public:
	BTTablePrimClass(char modo, char* nomeArq);
	~BTTablePrimClass(void);
	bool vazio(void) const;
	bool inserir(const TipoIndicePrim &item);
	bool recuperar(int chave, TipoIndicePrim &item);

private:
	bool procuraNo(const int buscado, int &local) const;
	void addItem(const TipoIndicePrim &novoItem, int direitaNova,
	 TipoNoPrim &no, int local);
	void split(const TipoIndicePrim &itemAtual, int direitaAtual,
		int raizAtual, int local, TipoIndicePrim &novoItem, 
		int &direitaNova);
	void coloca(const TipoIndicePrim &itemAtual, int raizAtual,
         bool &moveAcima, TipoIndicePrim &novoItem, int &direitaNova);
	int raiz;   // ponteiro para a raiz
	int numNos; // numero de nos na B-tree
	int tamNo;   // numero de bytes por no
	TipoNoPrim noAtual; //o no atual da B tree
};

