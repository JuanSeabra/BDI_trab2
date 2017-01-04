#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vector>
#include <boost/tokenizer.hpp>
using boost::tokenizer;
using boost::escaped_list_separator;
using boost::split;
using boost::is_any_of;
using namespace std;

#include "artigo.h"
//#include "hashing.h"

//TODO hashing file <waldomiro>
//     primary index file (B-Tree)
//     secondary index file (B-Tree)

void remove_all_chars(char* str, char c) {
	char *pr = str, *pw = str;
	while (*pr) {
		*pw = *pr++;
		pw += (*pw != c);
	}
	*pw = '\0';
}

vector<string> parse(string s){
	char aux[4096];
	strcpy(aux,s.c_str());
	remove_all_chars(aux,'\\');
	s = aux;
	typedef tokenizer<escaped_list_separator<char> > so_tokenizer;
	vector<string> strs;
	so_tokenizer tok(s, escaped_list_separator<char>('\\', ';', '\"'));
	for(so_tokenizer::iterator beg=tok.begin(); beg!=tok.end(); ++beg){
		strs.push_back(*beg);
	}
	return strs;
}


int main(int argc, char *argv[]){
	ifstream f;
	FILE *out;
	string linha,linha_aux;
	char tok[1024];
	Artigo registro;
	int count = 0;
	vector<string> strs;
	size_t maior_Nome = 0, maior_Titulo = 0, maior_snippet = 0;

	string aux;

	cout << sizeof(Artigo) << endl;

	if(argc != 2){
		cout << "Modo de uso ./upload <arquivo de entrada>" << endl;
		exit(0);
	}

	f.open(argv[1]);
	out = fopen("Artigo.dat","w");

	while (getline(f,linha)) {
		strs = parse(linha);

		/*Correção de erro de quebra de linha*/
		if (strs.size() < 7){
			getline(f,linha_aux);
			linha += linha_aux;
			strs = parse(linha);
		}
		
		strcpy(tok,strs[0].c_str());
		registro.id = atoi(tok);

		if (strs[1].size() > maior_Titulo) {
			maior_Titulo = strs[1].size();
		}

		strcpy(tok,strs[1].c_str());
		strncpy(registro.titulo,tok,250);

		strcpy(tok,strs[2].c_str());
		registro.ano = (unsigned short) atoi(tok);

		if (strs[3].size() > maior_Nome) {
			maior_Nome = strs[3].size();
			cout << strs[3] << endl;
		}

		strcpy(tok,strs[3].c_str());
		strncpy(registro.autores,tok,50);

		strcpy(tok,strs[4].c_str());
		registro.citacoes = (unsigned short) atoi(tok);

		strcpy(tok,strs[5].c_str());
		strncpy(registro.atualizacao,tok,20);

		if (strs[6].size() > maior_snippet) {
			maior_snippet = strs[6].size();
			aux = strs[6];
		}

		strcpy(tok,strs[6].c_str());
		strncpy(registro.snippet,tok,1024);

		/*Escreve arquivo de dados
		 * Escreve arquivo de indice primario
		 * Escreve arquivo de indice secundario*/
		count++;
	}

	cout << "Tamanho dos resgistros" << aux << endl;
	cout << "Foram escritos " << count << " blocos!" << endl;
	cout << "maior nome de autor: " << maior_Nome << endl
		<< "maior titulo: " << maior_Titulo << endl
		<< "maior snippet: " << maior_snippet << endl;
	return 0;
}
