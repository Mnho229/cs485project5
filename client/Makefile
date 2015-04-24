CFLAGS = -Wall
CPPFLAGS = -Wall

all: server mcput mcget mcdel mclist

csapp.h:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/include/csapp.h

csapp.c:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/src/csapp.c

csapp.o: csapp.h csapp.c

server: server.c csapp.o 
	gcc $(CPPFLAGS) server.c csapp.o -lpthread -lm -o server

mcput: mcput.cpp csapp.o 
	g++ $(CPPFLAGS) mcput.cpp csapp.o -lpthread -lm -o mcput

mcget: mcget.cpp csapp.o
	g++ $(CPPFLAGS) mcget.cpp csapp.o -lpthread -lm -o mcget

mcdel: mcdel.cpp csapp.o
	g++ $(CPPFLAGS) mcdel.cpp csapp.o -lpthread -lm -o mcdel

mclist: listfiles.cpp csapp.o
	g++ $(CPPFLAGS) listfiles.cpp csapp.o -lpthread -lm -o mclist

.PHONY: clean
clean:
	/bin/rm -rf csapp.h csapp.c *.o server mcput mcget mcdel mclist
