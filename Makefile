CC=g++
CFLAGS=-O0 -g
DEPS =hashing.h

%.o: %.cpp  $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

upload: upload.o hashing.o
	$(CC) -o $@ $^ $(CFLAGS)

clean: 
	rm *.o upload
