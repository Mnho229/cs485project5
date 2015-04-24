CFLAGS = -Wall
CPPFLAGS = -Wall
SDIR = server
CDIR = client

all: server mcput mcget mcdel mclist

server: $(SDIR)/server.c $(SDIR)/csapp.o
	gcc $(CPPFLAGS) $(SDIR)/server.c $(SDIR)/csapp.o -lpthread -lm -o serv 

mcput.o: $(CDIR)/mcput.cpp $(CDIR)/cloud_library.c
	g++ -c $(CDIR)/mcput.cpp $(CDIR)/cloud_library.c 

mcput: $(CDIR)/mcput.o $(CDIR)/csapp.o 
	g++ $(CPPFLAGS) $(CDIR)/mcput.o $(CDIR)/csapp.o  -lpthread -lm -o mcput 

mcget.o: $(CDIR)/mcget.cpp $(CDIR)/cloud_library.c
	g++ -c $(CDIR)/mcget.cpp $(CDIR)/cloud_library.c 

mcget: $(CDIR)/mcget.o $(CDIR)/csapp.o
	g++ $(CPPFLAGS) $(CDIR)/mcget.o $(CDIR)/csapp.o  -lpthread -lm -o mcget 

mcdel.o: $(CDIR)/mcdel.cpp $(CDIR)/cloud_library.c
	g++ -c $(CDIR)/mcdel.cpp $(CDIR)/cloud_library.c 

mcdel: $(CDIR)/mcdel.o $(CDIR)/csapp.o 
	g++ $(CPPFLAGS) $(CDIR)/mcdel.o $(CDIR)/csapp.o  -lpthread -lm -o mcdel 

mclistfiles.o: $(CDIR)/listfiles.cpp $(CDIR)/cloud_library.c
	g++ -c $(CDIR)/listfiles.cpp $(CDIR)/cloud_library.c 

mclist: $(CDIR)/listfiles.o $(CDIR)/csapp.o
	g++ $(CPPFLAGS) $(CDIR)/listfiles.o $(CDIR)/csapp.o -lpthread -lm -o mclist 

.PHONY: clean
clean:
	/bin/rm -rf csapp.h csapp.c *.o server mcput mcget mcdel mclist
