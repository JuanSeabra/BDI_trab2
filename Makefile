CC=g++
CFLAGS=-O3 -g -std=c++14
DEPS =hashing.h btree.h

%.o: %.cpp  $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

upload: upload.o hashing.o btree.o
	$(CC) -o $@ $^ $(CFLAGS)

clean: 
	rm *.o upload
