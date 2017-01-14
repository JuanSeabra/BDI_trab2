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
    Auxiliar(char* nomeArq);
    ~Auxiliar();
    int inserir(BlocoAuxiliar blocoAtual);
    void atualizar(int local, BlocoAuxiliar blocoAtual);
    BlocoAuxiliar recuperar(int local);

    fstream arqAux;
};
