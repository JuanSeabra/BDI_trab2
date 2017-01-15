#include "btree_prim.h"

/*
	Este arquivo contem a implementacao das funcoes da classe
	BTTablePrimClass como definidas em btree_prim.h
	Esta classe representa indices primarios
*/

/*
	Construtor da classe: recebe um modo de abertura e um nome de arquivo.
	Grava um cabecalho com as informacoes da arvore B.
	No modo de leitura, le este cabecalho.
*/
BTTablePrimClass::BTTablePrimClass(char modo, char* nomeArquivo) {
	openMode = modo;
	tamNo = sizeof(TipoNoPrim);

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
			raiz = pontNull;
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


		raiz = pontNull;
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
BTTablePrimClass::~BTTablePrimClass(void) {
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
bool BTTablePrimClass::vazio(void) const {
	return (raiz == pontNull);
}

//Verifica se um no esta na arvore B
bool BTTablePrimClass::procuraNo(const int buscado, int &local) const {
	bool achou = false;

	//cout << "Quantidade de chaves do no: " << noAtual.qtd << endl;
	if (buscado < noAtual.chave[0].id)
		local = -1;
	else {
		local = noAtual.qtd - 1;
		while( (buscado < noAtual.chave[local].id) && (local > 0)) {
			//cout << " " << noAtual.chave[local].id;
			local--;
		}
		//cout << endl;
		if (buscado == noAtual.chave[local].id)
			achou = true;
	}

	return achou;
}

//Insere um novo registro em um no
void BTTablePrimClass::addItem(const TipoIndicePrim &novoItem, int direitaNova,
	TipoNoPrim &no, int local) {
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
void BTTablePrimClass::split(const TipoIndicePrim &itemAtual, int direitaAtual,
		int raizAtual, int local, TipoIndicePrim &novoItem,
		int &direitaNova) {
	int i, meio;
	TipoNoPrim noDireita;

	if (local < minChaves)
		meio = minChaves;
	else meio = minChaves+1;

	arquivo.seekg(raizAtual * tamNo, ios::beg);
	arquivo.read(reinterpret_cast<char*> (&noAtual), tamNo);

	for (i = meio; i < maxChaves; i++) {
		//move metade dos itens para a direita
		noDireita.chave[i-meio] = noAtual.chave[i];
		noDireita.ponteiros[i-meio+1] = noAtual.ponteiros[i+1];
	}

	noDireita.qtd = maxChaves - meio;
	noAtual.qtd = meio; //sera incrementado com o novo item

	//coloca o item atual no lugar
	if (local < minChaves)
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
void BTTablePrimClass::coloca(const TipoIndicePrim &itemAtual, int raizAtual,
         bool &moveAcima, TipoIndicePrim &novoItem, int &direitaNova) {
	int local;

	//caso base
	if (raizAtual == pontNull) {
		moveAcima = true;
		novoItem = itemAtual;
		direitaNova = pontNull;

	}

	//passo recursivo
	else {
		arquivo.seekg(raizAtual * tamNo, ios::beg);
		arquivo.read(reinterpret_cast<char*> (&noAtual), tamNo);

		if(procuraNo(itemAtual.id, local)) {
			cerr << "Valor ja existe na arvore B!" << endl;
			moveAcima = false;
			exit(1);
		}

		coloca(itemAtual, noAtual.ponteiros[local+1], moveAcima, novoItem, direitaNova);
		if (moveAcima) {
			arquivo.seekg(raizAtual*tamNo, ios::beg);
			arquivo.read(reinterpret_cast<char*> (&noAtual), tamNo);

			if (noAtual.qtd < maxChaves) {
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
bool BTTablePrimClass::inserir(const TipoIndicePrim &item) {
	bool moveAcima;
	int direitaNova;
	TipoIndicePrim novoItem;

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
bool BTTablePrimClass::recuperar(int chave, TipoIndicePrim &item) {
	int raizAtual;
	int local;
	bool achou = false;
	raizAtual = raiz;

	while(raizAtual != pontNull && !achou) {
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
bool BTTablePrimClass::recuperarContBlocos(int chave, TipoIndicePrim &item, int& numBlocos) {
	int raizAtual;
	int local;
	bool achou = false;
	raizAtual = raiz;
	numBlocos = 0;

	while(raizAtual != pontNull && !achou) {
		arquivo.seekg(raizAtual*tamNo, ios::beg);
		arquivo.read(reinterpret_cast<char*> (&noAtual), tamNo);

		numBlocos++;
		if (procuraNo(chave, local)) {
			achou = true;
			item = noAtual.chave[local];
		}
		else
			raizAtual = noAtual.ponteiros[local+1];
	}

	return achou;
}
