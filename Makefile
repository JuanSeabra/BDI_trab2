CC=g++
CFLAGS=-O0 -g
DEPS =hashing.h

all: upload findrec

%.o: %.cpp  $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

upload: upload.o hashing.o
	$(CC) -o $@ $^ $(CFLAGS)

findrec: findrec.o hashing.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o upload findrec *.dat overflowFile bucketIndexes
