#include <fstream>
#include "tipoIndiceSec.h"

//Atributos
//Indice secundario
const int mS = 6; //ordem m da arvore B
const int maxChavesSec = 2*mS; //numero maximo de chaves
const int minChavesSec = mS;   //numero minimo de chaves
const int pontNullS = -1;  //ponteiro para null


//Estrutura dos nos
//Indice secundario
typedef struct {
	int qtd; //numero de chaves/pont atualmente no no
	TipoIndiceSec chave[maxChavesSec]; //chaves/ponteiros
	int ponteiros[maxChavesSec+1]; //apontadores da arvore B
}TipoNoSec;

//Classes
//Indice secundario
class BTTableSecClass {
public:
	BTTableSecClass(char modo, char* nomeArq);
	~BTTableSecClass(void);
	bool inserir(const TipoIndiceSec &item);
	bool recuperar(char* chave, TipoIndiceSec &item);
	bool recuperarContBlocos(char* chave, TipoIndiceSec &item, int& numBlocos);

	bool procuraNo(const char* buscado, int &local) const;
	void addItem(const TipoIndiceSec &novoItem, int direitaNova,
	 TipoNoSec &no, int local);
	void split(const TipoIndiceSec &itemAtual, int direitaAtual,
		int raizAtual, int local, TipoIndiceSec &novoItem,
		int &direitaNova);
	void coloca(const TipoIndiceSec &itemAtual, int raizAtual,
         bool &moveAcima, TipoIndiceSec &novoItem, int &direitaNova);
	int raiz;   // ponteiro para a raiz
	int numNos; // numero de nos na B-tree
	int tamNo;   // numero de bytes por no
	TipoNoSec noAtual; //o no atual da B tree

	fstream arquivo; //arquivo para os dados
	int numItens;	 //numero de registros primarios na tabela
	char openMode;	 // leitura (r) ou write(w)
};
