#include "csapp.h"
#define NAME_SIZE 80
#define FILE_SIZE 100000
#define REQUEST_SIZE 4
#define DNS_SIZE 40

void echo(int connfd)
{
	int n;
	char buf[MAXLINE];
	rio_t rio; 

	Rio_readinitb(&rio,connfd);
	while((n = Rio_readlineb(&rio,buf,MAXLINE))!= 0)
	{

		printf("server received %d bytes\n", n);
		printf("buff: %s\n", buf);
		Rio_writen(connfd,buf,n);
		printf("After rio_writen\n");
		//Rio_writen(connfd,"\nsomething",n) 
	} 
	printf("left echo\n");

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
    //strcpy(Secret_Key, argv[2]);
    listenfd = Open_listenfd(port);
    while (1) 
    {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

		echo(connfd);
		Close(connfd);
    }

    exit(0);
}


int checkKey(rio_t *rio, unsigned int SecretKey)
{
	size_t num;
	int key_size = (SecretKey == 0 ? 1 : (int)(log10(SecretKey)+1));
	char buf[key_size]; 
	unsigned int clientKey, netOrder; 

	if((num = Rio_readnb(rio,buf,key_size) == key_size)
	{
		memcpy(&netOrder,&buf,key_size); 
		clientKey = ntohl(netOrder); 

		printf("Secret Key  =  %d\n",clientKey); 
		if(clientKey == SecretKey)
		{
			return 0;
		}
		else
		{
			return -1; 
		}
	}
	return -1; 
}


int getRequest(rio_t *rio)
{
	size_t num; 
	char buffer[REQUEST_SIZE];
	unsigned int reqType, netOrder; 

	if((n = Rio_readnb(rio,buf,REQUEST_SIZE) == REQUEST_SIZE)
	{
		memcpy(&netOrder,&buf,REQUEST_SIZE); 
		reqType = ntohl(netOrder); 

		return reqType; 
	}
	return -1; 
}

int store(rio_t *rio, int connfd)
{
	size_t num; 
	char fileName[NAME_SIZE]; 
	char FileName[NAME_SIZE]; 
	char FileSize[4];
	unsigned int filesize, netOrder, stat, messageSize; 
	char data_in_file[FILE_SIZE];
	char  *data, *message; 
	FILE *file; 
	unsigned int maxBytes = 4; 

	if((n = Rio_readnb(rio, fileName, NAME_SIZE)) == NAME_SIZE) 
	{
        memcpy(&FileName, &fileName, NAME_SIZE);
        printf("Filename  = %s\n", fileName);
    } 
    else 
    {
        printf("Filename  = NONE\n");
        status = -1;
    }

    if((n = Rio_readnb(rio,FileSize,maxBytes)) == maxBytes)
    {
    	memcpy(&netOrder.&FileSize,maxBytes);
    	fileSize = ntohl(netOrder); 
    }
    else
    {
    	stat = -1; 
    }

    if((n = Rio_readnb(rio,data_in_file,filesize)) == filesize)
    {
    	data = (char*) malloc(sizeof(char)*filesize); 
    	if(data == NULL)
    	{
    		fprintf(stderr, "Memory Error\n"); 
    		return -1; 
    		memcpy(data,&data_in_file,filesize);
    	}
    }
    else
    {
    	status = -1; 
    }

    if((file = Fopen(fileName,"w")) != NULL)
    {
    	Fwrite(data, sizeof(char), filesize, file);
    	Fclose(file); 
    	if(addFileToList(filename) == 0)
    	{
    		stat = 0; 
    	} 
    	else
    	{
    		stat = -1; 
    	}
    }
    else
    {
    	stat = -1; 
    }

    free(data); 
    unsigned int currStat = 4; 
    messageSize = currStat;

    
    message = (char*) malloc (sizeof(char*)*messageSize);
    if(message == NULL) { fprintf(stderr, "Memory Error\n"); return -1; }
    char *messageBuf = message;

    
    netOrder = htonl(status);
    memcpy(messageBuf, &netOrder, currStat);
    messageBuf += currStat;


    Rio_writen(connfd, message, messageSize);
    free(message);

    return status;

}
int retrieveRequest(rio_t *rio, int connfd) 
{
	size_t num;
    char NameBuf[NAME_SIZE];
    char fileName[NAME_SIZE];
    unsigned int fileSize, netOrder, status, messageSize;
    char *data, *message;
    FILE *file;
    unsigned int currStat = 4;
    unsigned int maxBytes = 4;  
    if((num = Rio_readnb(rio, NameBuf, NAME_SIZE)) == NAME_SIZE) 
    {
        memcpy(&fileName, &NameBuf, NAME_SIZE);
        printf("Filename  = %s\n", fileName);
    } 
    else 
    {
        printf("Filename  = NONE\n");
        status = -1;
    }

    if(fileInList(fileName) == -1) 
    	{ 
    		fileSize = 0; 
    		status = -1; 
    	}
    else 
    {
        // Check if file exists and open it
        file = fopen(fileName, "r");
        if(file == 0) 
        	{ 
        		fprintf(stderr, "Cannot open input file\n"); 
        		fileSize = 0; 
        		status = -1; 
        	}
        else 
        {
            // Obtain file size
            fseek(file, 0, SEEK_END);
            fileSize = ftell(file);
            rewind(file);

            // Allocate memory for the data buffer
            data = (char*) malloc (sizeof(char)*fileSize);
            if(data == NULL) { fprintf(stderr, "Memory Error\n"); return -1; }

            // Copy file data into data buffer
            if((n = fread(data, 1, fileSize, file)) == fileSize) 
            	{ 
            		fclose(file); 
            		status = 0; 
            	}
            else 
            	{ 
            		fileSize = 0; 
            		status = -1; 
            	}
        }
    }

    // Set message size according to the protocol
    messageSize = currStat + maxBytes + fileSize;

    // Allocate memory for the message buffer defined by the protocol
    message = (char*) malloc (sizeof(char*)*messageSize);
    if(message == NULL) 
    	{ 
    		fprintf(stderr, "Memory Error\n"); 
    		return -1;
    	 }
    char *messageBuf = message;

    // Copy the operational status into message buffer
    netOrder = htonl(status);
    memcpy(messageBuf, &netOrder, currStat);
    messageBuf += currStat;

    // Copy the file size into message buffer
    netOrder = htonl(fileSize);
    memcpy(messageBuf, &netOrder, maxBytes);
    messageBuf += maxBytes;

    // Copy file data into message buffer
    memcpy(messageBuf, data, fileSize);
    messageBuf += fileSize;
    free(data);

    // Send the response message
    Rio_writen(connfd, message, messageSize);
    free(message);

    return status;
}