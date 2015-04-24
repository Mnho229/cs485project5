#include "csapp.h"
#define NAME_SIZE 80
#define FILE_SIZE 100000
#define REQUEST_SIZE 4
#define DNS_SIZE 40

unsigned int numFiles = 0;
char fileList[100][NAME_SIZE];

int deleteRequest(rio_t *rio, int connfd);
int listFilesRequest(rio_t *rio, int connfd);
int addFileToList(char *fileName);
int removeFileFromList(char *fileName);
int fileInList(char *fileName);
int validKey(rio_t *rio, unsigned int SecretKey);
int getRequest(rio_t *rio);
int store(rio_t *rio, int connfd);
int retrieveRequest(rio_t *rio, int connfd);
int deleteRequest(rio_t *rio, int connfd);

int main(int argc, char **argv) 
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct hostent;
	unsigned int secretKey;
    if (argc != 3) 
    {
		fprintf(stderr, "usage: %s <port> <secretKey>\n", argv[0]);
		exit(0);
    }

    port = atoi(argv[1]);
    secretKey = atoi(argv[2]);
    listenfd = Open_listenfd(port);
    while (1) 
    {
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

		int requestType = -1;
		int status = -1;
		rio_t rio;
		Rio_readinitb(&rio, connfd);
        int KeyCheck = checkKey(&rio, secretKey); 
		if (KeyCheck == 0) 
        {

			requestType = getRequest(&rio);

			if (requestType == 0) {
				printf("Request Type = get\n");
				status = retrieveRequest(&rio, connfd);
			}
			else if (requestType == 1) {
				printf("Request Type = put\n");
				status = store(&rio, connfd);
			}
			else if (requestType == 2) {
				printf("Request Type = del\n");
				status = deleteRequest(&rio, connfd);
			}
			else if (requestType == 3) {
				printf("Request Type = list\n");
				status = listFilesRequest(&rio, connfd);
			}
			else {
				printf("Request Type = invalid");
			}
		}

		if (status == 0) 
        {
			printf("Operation Status = Success\n");
		}
		else 
        {
			printf("Operation Status = error\n");
		}
		printf("-------------------------------\n");
		Close(connfd);
    }

    exit(0);
}

int deleteRequest(rio_t *rio, int connfd) {
    size_t n;
    char fileNameBuf[NAME_SIZE];
    char fileName[NAME_SIZE];
    unsigned int netOrder, stat, replySize;
    unsigned int currStat = 4;
    char *reply;   

    /*******************************
     * Delete Request Message      *
     *******************************/
    
    // Read file name
    if((n = Rio_readnb(rio, fileNameBuf, NAME_SIZE)) == NAME_SIZE) {
        // Copy binary data from buffer
        memcpy(&fileName, &fileNameBuf, NAME_SIZE);
        printf("Filename = %s\n", fileName);
    } 
    else {
        printf("Filename = NONE\n");
        stat = -1;
    }

    /*******************************
     * Delete Response Message     *
     *******************************/

    // Check if file is in list and remove it
    if(removeFileFromList(fileName) == -1) { 
    	stat = -1;
    }
    else {
        // Delete file
        if(remove(fileName) != 0) { stat = -1; }
        else { stat = 0; }
    }

    // Set reply size according to the protocol
    replySize = currStat;

    // Allocate memory for the reply buffer defined by the protocol
    reply = (char*) malloc (sizeof(char*)*replySize);
    if(reply == NULL) { 
    	fprintf(stderr, "Memory Error\n"); return -1;
    }

    char *replyPtr = reply;

    // Copy the operational stat into reply buffer
    netOrder = htonl(stat);
    memcpy(replyPtr, &netOrder, currStat);
    replyPtr += currStat;

    // Send the response reply
    Rio_writen(connfd, reply, replySize);
    free(reply);

    return stat;
}

// Return 0 if the listing is successful, -1 if fails
int listFilesRequest(rio_t *rio, int connfd) {
    unsigned int datalen, netOrder, stat, replySize;
    unsigned int currStat = 4;
    unsigned int maxBytes = 4;
    char *reply;

    /*******************************
     * List Files Response Message *
     *******************************/

    // Obtain data length
    datalen = numFiles * NAME_SIZE;

    // Set reply size according to the protocol
    replySize = currStat + maxBytes + datalen;

    // Allocate memory for the reply buffer defined by the protocol
    reply = (char*) malloc (sizeof(char*)*replySize);

    if(reply == NULL) {
    	fprintf(stderr, "Memory Error\n"); 
    	return -1; 
    }
    char *replyPtr = reply;
    stat = 0;

    // Copy the operational status into reply buffer
    netOrder = htonl(stat);
    memcpy(replyPtr, &netOrder, currStat);
    replyPtr += currStat;

    // Copy the data length into reply buffer
    netOrder = htonl(datalen);
    memcpy(replyPtr, &netOrder, maxBytes);
    replyPtr += maxBytes;

    // Copy file data into reply buffer
    memcpy(replyPtr, fileList, datalen);
    replyPtr += datalen;

    // Send the response reply
    Rio_writen(connfd, reply, replySize);
    free(reply);

    return stat;
}

// Adds the file name to the last element of the file list
// Returns 0 if the file is added to the list, -1 if not added (already in list or list full)
int addFileToList(char *fileName) {
    if( (fileInList(fileName)) == -1 && (numFiles < 100)) {

        strcpy(fileList[numFiles], fileName);
        numFiles++;
        return 0;
    }
    return -1;
}

// Removes the file name from the file list and makes remaining elements continuous
// Returns 0 if the file is removed from the list, -1 if not removed (not found in list)
int removeFileFromList(char *fileName) {
    int i;
    if (((i = fileInList(fileName)) != -1) && (i < 100 - 1)) {
        // Overwrite file name and move all remaining elements
        memmove(fileList[i], fileList[i+1], numFiles * NAME_SIZE);
        numFiles--;
        return 0;
    } 
    else if (i == 100 - 1) {
        // If last element, just drop the index
        numFiles--;
        return 0;
    }
    return -1;
}

// Returns the index of the file if in the list, -1 if not in list
int fileInList(char *fileName) {
    int i;
    for(i = 0; i < numFiles; i++) {
        if(strcmp(fileList[i], fileName) == 0) { return i; }
    }
    return -1;
}

int checkKey(rio_t *rio, unsigned int SecretKey)
{
	size_t num;
	int key_size = (SecretKey == 0 ? 1 : (int)(log10(SecretKey)+1));
	char buf[key_size]; 
	unsigned int clientKey, netOrder; 

	if((num = Rio_readnb(rio,buf,key_size)) == key_size)
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

	if((num = Rio_readnb(rio,buffer,REQUEST_SIZE)) == REQUEST_SIZE)
	{

		memcpy(&netOrder,&buffer,REQUEST_SIZE);
		reqType = ntohl(netOrder); 

		return reqType; 
	}
	return -1; 
}

int store(rio_t *rio, int connfd)
{
	size_t num; 
	char nameBuf[NAME_SIZE]; 
	char FileName[NAME_SIZE]; 
	char FileSize[4];
	unsigned int filesize, netOrder, stat, messageSize; 
	char data_in_file[FILE_SIZE];
	char  *data, *message; 
	FILE *file; 
	unsigned int maxBytes = 4; 

	if((num = Rio_readnb(rio, nameBuf, NAME_SIZE)) == NAME_SIZE) 
	{
        memcpy(&FileName, &nameBuf, NAME_SIZE);
        printf("Filename  = %s\n", FileName);
    } 
    else 
    {
        printf("Filename  = NONE\n");
        stat = -1;
    }

    if((num = Rio_readnb(rio,FileSize,maxBytes)) == maxBytes)
    {
    	memcpy(&netOrder,&FileSize,maxBytes);
    	filesize = ntohl(netOrder); 
    }
    else
    {
    	stat = -1; 
    }

    if((num = Rio_readnb(rio,data_in_file,filesize)) == filesize)
    {
    	data = (char*) malloc(sizeof(char)*filesize); 
    	if(data == NULL)
    	{
    		fprintf(stderr, "Memory Error\n"); 
    		return -1; 
    	}
    	memcpy(data,&data_in_file,filesize);
    }
    else
    {
    	stat = -1; 
    }

    if((file = Fopen(nameBuf,"w")) != NULL)
    {
    	Fwrite(data, sizeof(char), filesize, file);
    	Fclose(file); 
    	if(addFileToList(FileName) == 0)
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

    
    netOrder = htonl(stat);
    memcpy(messageBuf, &netOrder, currStat);
    messageBuf += currStat;

    Rio_writen(connfd, message, messageSize);
    free(message);

    return stat;

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
            if((num = fread(data, 1, fileSize, file)) == fileSize) 
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