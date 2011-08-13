CC=gcc
CFLAGS= -W -Wall -g -pedantic -lsocket -lnsl

all: time-client

time-client: error.o udp.o confprocess.o time-client.o
	$(CC) -o time-client confprocess.o error.o udp.o time-client.o $(CFLAGS)

error.o: error.c
	$(CC) -c error.c $(CFLAGS)

udp.o: udp.c
	$(CC) -c udp.c $(CFLAGS)

confprocess.o: confprocess.c
	$(CC) -c confprocess.c $(CFLAGS)

time-client.o: time-client.c 
	$(CC) -c time-client.c $(CFLAGS)

clean:
	-rm *.o
	-rm time-client
	-rm core
	-rm recvlog
	-rm sendlog

cleanlog:
	-rm recvlog
	-rm sendlog
