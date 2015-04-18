#include <iostream>
#include "csapp.h"
#define MAXLINE 80; 
using namespace std; 
void echo(int connfd)
{
	size_t n;
	char buf[MAXLINE];
	rio_t rio; 

	Rio_readinitb(&rio,connfd);
	while(n = Rio_readlineb(&rio,buf,MAXLINE)!= 0)
	{
		cout << "Server received " << n << " bytes" << endl; 
		Rio_written(confd,buf,n); 
	} 

}


int main(int argc, char **argv) 
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;
	char *Secret_Key;     
    if (argc != 3) 
    {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
    }

    port = atoi(argv[1]);
    strcpy() 
    listenfd = Open_listenfd(port);
    while (1) 
    {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

		/* Determine the domain name and IP address of the client */
		hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);
		printf("server connected to %s (%s)\n", hp->h_name, haddrp);

		echo(connfd);
		Close(connfd);
    }

    exit(0);
}