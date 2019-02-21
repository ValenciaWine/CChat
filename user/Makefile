CFLAGS=-Wall

all: test_user



common.o: common.h

user.o: common.h user.h

test_user.o: user.h

test_user: test_user.o user.o common.o
	$(CC) -o $@ test_user.o user.o common.o -lpthread


clean:
	rm *.o test_user
