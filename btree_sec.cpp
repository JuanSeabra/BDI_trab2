#include "btree_sec.h"
#include <string.h>

/*
	Este arquivo contem a implementacao das funcoes da classe
	BTTableSecClass como definidas em btree_sec.h
	Esta classe representa indices secundarios
*/

/*
	Construtor da classe: recebe um modo de abertura e um nome de arquivo.
	Grava um cabecalho com as informacoes da arvore B.
	No modo de leitura, le este cabecalho.
*/
BTTableSecClass::BTTableSecClass(char modo, char* nomeArquivo) {
	openMode = modo;
	tamNo = sizeof(TipoNoSec);

	//modo de leitura
	if (modo == 'r') {
		arquivo.open(nomeArquivo, ios::in | ios::binary);
		if (arquivo.fail()) {
			cerr << "Erro ao abrir o arquivo" << endl;
			exit(1);
		}

		arquivo.read(reinterpret_cast <char *> (&noAtual), tamNo);
		if (arquivo.fail()) {
			//se nao eh possivel ler deste arquivo a B tree esta vazia
			numItens = 0;
			numNos = 0;
			raiz = pontNullS;
		}
		else { //o no 0 contem dados - cabecalho da arvore
			numItens = noAtual.ponteiros[0];
			numNos = noAtual.ponteiros[1];
			raiz = noAtual.ponteiros[2];
		}
	}

	//modo de escrita
	else if (modo == 'w') {
		arquivo.open(nomeArquivo, ios::in | ios::out | ios::trunc | ios::binary);
		if (arquivo.fail()) {
			cerr << "Erro ao abrir o arquivo" << endl;
			exit(1);
		}


		raiz = pontNullS;
		numItens = 0;
		numNos = 0;
		noAtual.ponteiros[0] = numItens;
		noAtual.ponteiros[1] = numNos;
		noAtual.ponteiros[2] = raiz;

		arquivo.seekp(0, ios::beg);
		arquivo.write(reinterpret_cast <char*> (&noAtual), tamNo);

	}
}

/*
	Destrutor: atualiza o cabecalho e fecha o arquivo
*/
BTTableSecClass::~BTTableSecClass(void) {
	if (openMode == 'w') {
		//atualizar o cabecalho
		noAtual.ponteiros[0] = numItens;
		noAtual.ponteiros[1] = numNos;
		noAtual.ponteiros[2] = raiz;

		arquivo.seekp(0, ios::beg);
		arquivo.write(reinterpret_cast <char*> (&noAtual), tamNo);
	}

	arquivo.close();
}

//Informa se a arvore esta vazia
bool BTTableSecClass::vazio(void) const {
	return (raiz == pontNullS);
}

//Verifica se um no esta na arvore B
bool BTTableSecClass::procuraNo(const char* buscado, int &local) const {
	bool achou = false;

	//cout << "Quantidade de chaves do no: " << noAtual.qtd << endl;
	if (strcmp(buscado, noAtual.chave[0].titulo) < 0)
		local = -1;
	else {
		local = noAtual.qtd - 1;
		while( (strcmp(buscado, noAtual.chave[local].titulo) < 0) && (local > 0)) {
			local--;
		}
		//cout << endl;
		if (strcmp(buscado, noAtual.chave[local].titulo) == 0)
			achou = true;
	}

	return achou;
}

//Insere um novo registro em um no
void BTTableSecClass::addItem(const TipoIndiceSec &novoItem, int direitaNova,
	TipoNoSec &no, int local) {
	int i;

	for (i = no.qtd; i > local; i--) {
		no.chave[i] = no.chave[i-1];
		no.ponteiros[i+1] = no.ponteiros[i];
	}

	no.chave[local] = novoItem;
	no.ponteiros[local+1] = direitaNova;
	no.qtd++;
}

//Realiza split em um no
void BTTableSecClass::split(const TipoIndiceSec &itemAtual, int direitaAtual,
		int raizAtual, int local, TipoIndiceSec &novoItem,
		int &direitaNova) {
	int i, meio;
	TipoNoSec noDireita;

	if (local < minChavesSec)
		meio = minChavesSec;
	else meio = minChavesSec+1;

	arquivo.seekg(raizAtual * tamNo, ios::beg);
	arquivo.read(reinterpret_cast<char*> (&noAtual), tamNo);

	for (i = meio; i < maxChavesSec; i++) {
		//move metade dos itens para a direita
		noDireita.chave[i-meio] = noAtual.chave[i];
		noDireita.ponteiros[i-meio+1] = noAtual.ponteiros[i+1];
	}

	noDireita.qtd = maxChavesSec - meio;
	noAtual.qtd = meio; //sera incrementado com o novo item

	//coloca o item atual no lugar
	if (local < minChavesSec)
		addItem(itemAtual, direitaAtual, noAtual, local+1);
	else addItem(itemAtual, direitaAtual, noDireita, local-meio+1);

	novoItem = noAtual.chave[noAtual.qtd - 1];
	noDireita.ponteiros[0] = noAtual.ponteiros[noAtual.qtd];
	noAtual.qtd--;

	arquivo.seekp(raizAtual * tamNo, ios::beg);
	arquivo.write(reinterpret_cast<char*> (&noAtual), tamNo);

	numNos++;
	direitaNova = numNos;
	arquivo.seekp(direitaNova * tamNo, ios::beg);
	arquivo.write(reinterpret_cast<char*> (&noDireita), tamNo);
}

//adiciona um item a tabela
void BTTableSecClass::coloca(const TipoIndiceSec &itemAtual, int raizAtual,
         bool &moveAcima, TipoIndiceSec &novoItem, int &direitaNova) {
	int local;

	//caso base
	if (raizAtual == pontNullS) {
		moveAcima = true;
		novoItem = itemAtual;
		direitaNova = pontNullS;

	}

	//passo recursivo
	else {
		arquivo.seekg(raizAtual * tamNo, ios::beg);
		arquivo.read(reinterpret_cast<char*> (&noAtual), tamNo);

		if(procuraNo(itemAtual.titulo, local)) {
			cerr << "Valor ja existe na arvore B!" << endl;
			moveAcima = false;
			exit(1);
		}

		coloca(itemAtual, noAtual.ponteiros[local+1], moveAcima, novoItem, direitaNova);
		if (moveAcima) {
			arquivo.seekg(raizAtual*tamNo, ios::beg);
			arquivo.read(reinterpret_cast<char*> (&noAtual), tamNo);

			if (noAtual.qtd < maxChavesSec) {
				moveAcima = false;
				addItem(novoItem, direitaNova, noAtual, local+1);
				arquivo.seekp(raizAtual*tamNo, ios::beg);
				arquivo.write(reinterpret_cast<char*> (&noAtual), tamNo);
			}
			else {
				moveAcima = true;
				split(novoItem, direitaNova, raizAtual, local, novoItem, direitaNova);
			}
		}

	}
}

//insere um novo item na arvore B
bool BTTableSecClass::inserir(const TipoIndiceSec &item) {
	bool moveAcima;
	int direitaNova;
	TipoIndiceSec novoItem;

	coloca(item, raiz, moveAcima, novoItem, direitaNova);

	//cria uma nova raiz
	if (moveAcima) {
		noAtual.qtd = 1;
		noAtual.chave[0] = novoItem;
		noAtual.ponteiros[0] = raiz;
		noAtual.ponteiros[1] = direitaNova;
		numNos++;
		raiz = numNos;
		arquivo.seekp(numNos * tamNo, ios::beg);
		arquivo.write(reinterpret_cast<char*> (&noAtual), tamNo);
	}

	numItens++;
	return true;
}

/*
	Verifica se um elemento esta na B tree
*/
bool BTTableSecClass::recuperar(char* chave, TipoIndiceSec &item) {
	int raizAtual;
	int local;
	bool achou = false;
	raizAtual = raiz;

	while(raizAtual != pontNullS && !achou) {
		arquivo.seekg(raizAtual*tamNo, ios::beg);
		arquivo.read(reinterpret_cast<char*> (&noAtual), tamNo);

		//cout << raizAtual << endl;
		if (procuraNo(chave, local)) {
			achou = true;
			item = noAtual.chave[local];
		}
		else
			raizAtual = noAtual.ponteiros[local+1];
	}

	return achou;
}

/*
	Verifica se um elemento esta na B tree
*/
bool BTTableSecClass::recuperarContBlocos(char* chave, TipoIndiceSec &item, int &numBlocos) {
	int raizAtual;
	int local;
	bool achou = false;
	raizAtual = raiz;

	while(raizAtual != pontNullS && !achou) {
		arquivo.seekg(raizAtual*tamNo, ios::beg);
		arquivo.read(reinterpret_cast<char*> (&noAtual), tamNo);

		numBlocos++;
		//cout << raizAtual << endl;
		if (procuraNo(chave, local)) {
			achou = true;
			item = noAtual.chave[local];
		}
		else
			raizAtual = noAtual.ponteiros[local+1];
	}

	return achou;
}
