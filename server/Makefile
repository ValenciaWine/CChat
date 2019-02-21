CFLAGS=-Wall

all: server

server.o: common.h
common.o: common.h

server: server.o common.o
	$(CC) -o $@ server.o common.o -lpthread

clean:
	rm -f *.o server
