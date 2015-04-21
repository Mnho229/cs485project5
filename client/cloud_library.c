#include "csapp.h"
#define NAME_SIZE 80
#define FILE_SIZE 100000
#define REQUEST_SIZE 4 

#define DNS_SIZE 40
int mycloud_putfile(char *MachineName, int TCPport, int SecretKey, char *Filename, char *data, int datalen)
{
	int clientfd; 
	char *message; 
	int key_size = (SecretKey == 0 ? 1 : (int)(log10(SecretKey)+1)); // find the length of the secret key  
	unsigned int messageSize, netOrder; 

	messageSize = key_size +  strlen(Filename) + FILE_SIZE + NAME_SIZE + datalen;  //request type size

	message = (char*) malloc(sizeof(char)*messageSize); 
	if(message == NULL)
	{
		printf(stderr, "Memory Error -> mcputs\n"); 
	} 
	char *message_buf = message; 

	// This copies the secret key to the message buffer. 
	netOrder = htonl(SecretKey);  // this puts the secret key from hostlong to host byte order. 
	memcpy(message_buf,&netOrder,key_size); 
	message_buf += key_size;

	//This copies the filename into the message buffer. 
	memcpy(message_buf,Filename,NAME_SIZE); 
	message_buf += NAME_SIZE; 

	//Copies the file data into the message
	memcpy(message_buf,data,datalen); 
	message_buf += datalen; 

	clientfd = Open_clientfd(MachineName,TCPport); 
	Rio_writen(clientfd,message,messageSize); 
	free(message); 


	size_t num; 
	char status_buf[4];
	unsigned int status; 
	rio_t rio; 
	Rio_readinitb(&rio,clientfd); 

	status = -1; 
	if(n = Rio_readnb(&rio,status_buf,4) == 4)
	{
		memcpy(&netOrder,&status_buf,4);
		status = ntohl(netOrder); 
	}
	Close(clientfd);
	return status; 
	



}























































/*int mycloud_getfile(char *MachineName, int TCPport, int SecretKey, char *Filename, char *data, int datalen)
{

}
int mycloud_delfile(char *MachineName, int TCPport, int SecretKey, char *Filename)
{

}
int mycloud_listfiles(char *MachineName, int TCPport, int SecretKey, char *listbuf, int listbuflen)
{
    
}*/