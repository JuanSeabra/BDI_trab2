#include "tipoIndicePrim.h"
#include <fstream>

//Atributos
//Indice primario
const int mP = 170; //ordem m da arvore B
const int maxChaves = 2*mP; //numero maximo de chaves
const int minChaves = mP;   //numero minimo de chaves

const int pontNull = -1;  //ponteiro para null

//Estrutura dos nos
//Indice primario
typedef struct {
	int qtd; //numero de chaves/pont atualmente no no
	TipoIndicePrim chave[maxChaves]; //chaves/ponteiros
	int ponteiros[maxChaves+1]; //apontadores da arvore B
}TipoNoPrim;

//Classes
//Indice primario
class BTTablePrimClass {
public:
	BTTablePrimClass(char modo, char* nomeArq);
	~BTTablePrimClass(void);
	bool vazio(void) const;
	bool inserir(const TipoIndicePrim &item);
	bool recuperar(int chave, TipoIndicePrim &item);

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

	fstream arquivo; //arquivo para os dados
	int numItens;	 //numero de registros primarios na tabela
	char openMode;	 // leitura (r) ou write(w)
};
