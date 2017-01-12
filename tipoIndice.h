#include <iostream>
#include <fstream>

using namespace std;

const int tamTitulo = 100;

typedef struct {
	int id;
	int pontBucket;
}TipoIndicePrim;

typedef struct {
	char titulo[tamTitulo];
	int pontBucket;
}TipoIndiceSec;