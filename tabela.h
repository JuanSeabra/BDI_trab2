#include "tipoIndice.h"
#include <fstream>

class TabelaBasePrimClass {
public:
	virtual bool vazio(void) const = 0;
	virtual bool inserir(const TipoIndicePrim &item) = 0;
	virtual bool recuperar(int chave, TipoIndicePrim &item) = 0;
protected:
	fstream arquivo; //arquivo para os dados
	int numItens;	 //numero de registros primarios na tabela
	char openMode;	 // leitura (r) ou write(w)
};