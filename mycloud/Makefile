CC = gcc
FLAGS = -Wall
LIBS = -pthread
OBJS = mcdel mcput mclist mcget mycloudServer
SRCS = mcdel.c mcput.c mclist.c mcget.c mycloudServer.c


all: clean mcdel mcput mclist mcget mycloudServer

mcdel: client/mcdel.o
	$(CC) $(FLAGS) $(LIBS) -o client/mcdel client/mcdel.c csapp.c
mcput: client/mcput.o 
	$(CC) $(FLAGS) $(LIBS) -o client/mcput client/mcput.c csapp.c
mclist: client/mclist.o
	$(CC) $(FLAGS) $(LIBS) -o client/mclist client/mclist.c csapp.c
mcget: client/mcget.o
	$(CC) $(FLAGS) $(LIBS) -o client/mcget client/mcget.c csapp.c
mycloudServer : server/mycloudServer.o
	$(CC) $(FLAGS) $(LIBS) -o server/mycloudServer server/mycloudServer.c csapp.c

clean:
	rm -f $(OBJS) client/*.o
	rm -f $(OBJS) server/*.o
