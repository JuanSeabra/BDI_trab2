#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vector>


#include "artigo.h"

void remove_all_chars(char* str, char c) {
	char *pr = str, *pw = str;
	while (*pr) {
		*pw = *pr++;
		pw += (*pw != c);
	}
	*pw = '\0';
}

int main(int argc, char *argv[]){
	using namespace std;
	using namespace boost;
	ifstream f;
	FILE *out;
	string linha;
	char tok[1024];
	Artigo bloco[3];
	int blocIndex = 0;
	vector<string> strs;

	if(argc != 2){
		cout << "Modo de uso ./upload <arquivo de entrada>" << endl;
		exit(0);
	}

	f.open(argv[1]);
	out = fopen("Artigo.dat","w");

	while (getline(f,linha)) {
		split(strs,linha,is_any_of(";"));

		if (strs.size() == 7){
			//tok = strtok(linha,";");
			strcpy(tok,strs[0].c_str());
			if(tok) remove_all_chars(tok,'\"');
			//		cout << "id " << tok << endl;
			bloco[blocIndex].id = atoi(tok);

			//tok = strtok(NULL,";");
			strcpy(tok,strs[1].c_str());
			if(tok) remove_all_chars(tok,'\"');
			//		cout << "titulo " << tok << endl;
			strncpy(bloco[blocIndex].titulo,tok,250);

			//tok = strtok(NULL,";");
			strcpy(tok,strs[2].c_str());
			if(tok) remove_all_chars(tok,'\"');
			//		cout << "ano " << tok << endl;
			bloco[blocIndex].ano = (unsigned short) atoi(tok);

			//tok = strtok(NULL,";");
			strcpy(tok,strs[3].c_str());
			if(tok) remove_all_chars(tok,'\"');
			//		cout << "autores " << tok << endl;
			strncpy(bloco[blocIndex].autores,tok,50);

			//tok = strtok(NULL,";");
			strcpy(tok,strs[4].c_str());
			if(tok) remove_all_chars(tok,'\"');
			//		cout << "citacoes " << tok << endl;
			bloco[blocIndex].citacoes = (unsigned short) atoi(tok);

			//tok = strtok(NULL,";");
			strcpy(tok,strs[5].c_str());
			if(tok) remove_all_chars(tok,'\"');
			//		cout << "atualizacao " << tok << endl;
			strncpy(bloco[blocIndex].atualizacao,tok,20);

			//tok = strtok(NULL,";");
			strcpy(tok,strs[6].c_str());
			if(tok) remove_all_chars(tok,'\"');
			//		cout << "snippet " << tok << endl;
			strncpy(bloco[blocIndex].snippet,tok,1024);

			blocIndex++;
			if(blocIndex == 3){
				fwrite(bloco,sizeof(Artigo),sizeof(Artigo)*blocIndex,out);
				blocIndex = 0;
			}
		}
	}
	if(blocIndex != 0){
		fwrite(bloco,sizeof(Artigo),sizeof(Artigo)*blocIndex,out);
	}
	return 0;
}
