CC=g++
CFLAGS=-O3 -g -std=c++14
DEPS =hashing.h btree_prim.h btree_sec.h auxiliar.h
%.o: %.cpp  $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

upload: upload.o hashing.o btree_prim.o btree_sec.o auxiliar.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o upload
