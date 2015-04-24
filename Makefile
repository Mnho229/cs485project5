CFLAGS = -Wall
CPPFLAGS = -Wall
SDIR = server
CDIR = client

all: server mcput mcget mcdel mclist

server: $(SDIR)/server.c $(SDIR)/csapp.o
	gcc $(CPPFLAGS) $(SDIR)/server.c $(SDIR)/csapp.o -lpthread -lm -o serv 

mcput.o: $(CDIR)/mcput.cpp $(CDIR)/cloud_library.c
	g++ -c $(CDIR)/mcput.cpp $(CDIR)/cloud_library.c 

mcput: $(CDIR)/mcput.cpp $(CDIR)/csapp.o $(CDIR)/cloud_library.c
	g++ $(CPPFLAGS) $(CDIR)/mcput.cpp $(CDIR)/csapp.o  -lpthread -lm -o mcput 

mcget.o: $(CDIR)/mcget.cpp $(CDIR)/cloud_library.c
	g++ -c $(CDIR)/mcget.cpp $(CDIR)/cloud_library.c 

mcget: $(CDIR)/mcget.cpp $(CDIR)/csapp.o $(CDIR)/cloud_library.c
	g++ $(CPPFLAGS) $(CDIR)/mcget.cpp $(CDIR)/csapp.o  -lpthread -lm -o mcget 

mcdel.o: $(CDIR)/mcdel.cpp $(CDIR)/cloud_library.c
	g++ -c $(CDIR)/mcdel.cpp $(CDIR)/cloud_library.c 

mcdel: $(CDIR)/mcdel.cpp $(CDIR)/csapp.o $(CDIR)/cloud_library.c
	g++ $(CPPFLAGS) $(CDIR)/mcdel.cpp $(CDIR)/csapp.o  -lpthread -lm -o mcdel 

mclistfiles.o: $(CDIR)/mclistfiles.cpp $(CDIR)/cloud_library.c
	g++ -c $(CDIR)/mclistfiles.cpp $(CDIR)/cloud_library.c 

mclist: $(CDIR)/listfiles.cpp $(CDIR)/csapp.o $(CDIR)/cloud_library.c
	g++ $(CPPFLAGS) $(CDIR)/listfiles.cpp $(CDIR)/csapp.o -lpthread -lm -o mclist 

.PHONY: clean
clean:
	/bin/rm -rf csapp.h csapp.c *.o server mcput mcget mcdel mclist
