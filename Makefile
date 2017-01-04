CC=g++
CFLAGS=-O3 -g
DEPS =hashing.h mumurhash.h

%.o: %.cpp  $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

upload: upload.o hashing.o murmurhash.o
	$(CC) -o $@ $^ $(CFLAGS)

clean: 
	rm *.o upload
