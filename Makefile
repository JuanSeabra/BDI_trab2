CC=g++
CFLAGS=-O3 -g -std=c++14
DEPS =hashing.h btree_prim.h btree_sec.h auxiliar.h
%.o: %.cpp  $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: upload seek1 seek2 findrec

upload: upload.o hashing.o btree_prim.o btree_sec.o auxiliar.o
	$(CC) -o $@ $^ $(CFLAGS)

seek1: seek1.o hashing.o btree_prim.o btree_sec.o auxiliar.o
	$(CC) -o $@ $^ $(CFLAGS)

seek2: seek2.o hashing.o btree_prim.o btree_sec.o auxiliar.o
	$(CC) -o $@ $^ $(CFLAGS)

findrec: findrec.o hashing.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o upload seek1 seek2
