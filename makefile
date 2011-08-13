CC=gcc
CFLAGS= -W -Wall -g -pedantic #-lsocket -lnsl

all: time-server

time-server: error.o udp.o confprocess.o time-server.o
	$(CC) -o time-server confprocess.o error.o udp.o time-server.o $(CFLAGS)

error.o: error.c
	$(CC) -c error.c $(CFLAGS)

udp.o: udp.c
	$(CC) -c udp.c $(CFLAGS)

confprocess.o: confprocess.c
	$(CC) -c confprocess.c $(CFLAGS)

time-server.o: time-server.c 
	$(CC) -c time-server.c $(CFLAGS)

clean:
	-rm *.o
	-rm time-server
	-rm core
	-rm recvlog
	-rm sendlog

cleanlog:
	-rm recvlog
	-rm sendlog
