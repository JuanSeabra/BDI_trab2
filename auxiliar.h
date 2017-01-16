#include <iostream>
#include <fstream>

using namespace std;

typedef struct {
  int pontBucket[511];
  int posBucket[511];
  int cont;
}BlocoAuxiliar;

class Auxiliar {
public:
    Auxiliar(char modo, char* nomeArq);
    ~Auxiliar();
    unsigned inserir(BlocoAuxiliar blocoAtual);
    void atualizar(unsigned local, BlocoAuxiliar blocoAtual);
    BlocoAuxiliar recuperar(unsigned local);

    fstream arqAux;
    char modoAbert;
};
