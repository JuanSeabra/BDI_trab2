#include "auxiliar.h"

/*
    Construtor. Recebe um modo de leitura e abre o arquivo 
    para leitura ou escrita.
*/
Auxiliar::Auxiliar(char modo, char* nomeArq) {
    modoAbert = modo;

    if (modo == 'w') {
        arqAux.open(nomeArq, ios::in | ios::out | ios::trunc | ios::binary);

        if (arqAux.fail()) {
            cerr << "Erro ao abrir o arquivo" << endl;
            exit(1);
        }
    }
    else if (modo == 'r') {
        arqAux.open(nomeArq, ios::in | ios::binary);
        if (arqAux.fail()) {
            cerr << "Erro ao abrir o arquivo" << endl;
            exit(1);
        }
    }
}

//Destrutor. Fecha o arquivo.
Auxiliar::~Auxiliar() {
    arqAux.close();
}

//Insere um novo bloco no arquivo de auxiliares
unsigned Auxiliar::inserir(BlocoAuxiliar blocoAtual) {
    unsigned local;

    blocoAtual.cont = 1;
    arqAux.seekp(0,ios::end);
    local = arqAux.tellp();
    arqAux.write(reinterpret_cast <char*> (&blocoAtual), sizeof(BlocoAuxiliar));

    return local;
}

//Atualiza um bloco auxiliar ja existente
void Auxiliar::atualizar(unsigned local, BlocoAuxiliar blocoAtual) {
    blocoAtual.cont++;
    arqAux.seekp(local, ios::beg);
    arqAux.write(reinterpret_cast <char*> (&blocoAtual), sizeof(BlocoAuxiliar));
}

//Obtem um bloco que ja esta presente no arquivo
BlocoAuxiliar Auxiliar::recuperar(unsigned local) {
    BlocoAuxiliar blocoLido;

    arqAux.seekg(local, ios::beg);
    arqAux.read(reinterpret_cast <char*> (&blocoLido), sizeof(BlocoAuxiliar));
    return blocoLido;
}
