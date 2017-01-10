#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <boost/tokenizer.hpp>
using boost::tokenizer;
using boost::escaped_list_separator;
using boost::split;
using boost::is_any_of;
using boost::erase_all;
using namespace std;

#include "artigo.h"
#include "hashing.h"

//TODO hashing file <waldomiro>
//     primary index file (B-Tree)
//     secondary index file (B-Tree)

vector<string> parse(string s){
	erase_all(s,"\\");
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
	Artigo registro;
	int count = 0, bucket_count = 0;
	vector<string> strs;


	cout << sizeof(Artigo) << endl;

	if(argc != 2){
		cout << "Modo de uso ./upload <arquivo de entrada>" << endl;
		exit(0);
	}

	f.open(argv[1]);
	out = fopen("Artigo.dat","w+");

	HashBuckets hash_registros (out,"overflowFile",145920);

	while (getline(f,linha)) {
		strs = parse(linha);

		//Correção de erro de quebra de linha
		if (strs.size() < 7){
			getline(f,linha_aux);
			linha += linha_aux;
			strs = parse(linha);
		}

		registro.id = stoi(strs[0]);


		strncpy(registro.titulo,strs[1].c_str(),300);

		registro.ano = (unsigned short) stoi(strs[2]);

		strncpy(registro.autores,strs[3].c_str(),100);

		registro.citacoes = (unsigned) stoi(strs[4]);

		strncpy(registro.atualizacao,strs[5].c_str(),19);


		strncpy(registro.snippet,strs[6].c_str(),100);

		//Escreve arquivo de dados
		//Escreve arquivo de indice primario
		//Escreve arquivo de indice secundario
		count++;
		if (count%7 == 0) bucket_count++;
		strs.clear();
		hash_registros.insert(registro);
		cout << "inseridos " << count << " registros até'agora!!" << endl;
	}
	f.close();
	fclose(out);
	cout << "Foram escritos " << count << " registros!" << endl
		<< "numero de buckets " << bucket_count << endl;
	return 0;
}
