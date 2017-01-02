CC=g++
CFLAGS=-O3 -lpqxx -lpq -g
#DEPS =bancoDeDados.h

%.o: %.cpp # $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

upload: upload.o #bancoDeDados.o
	$(CC) -o $@ $^ $(CFLAGS)

clean: 
	rm *.o upload
