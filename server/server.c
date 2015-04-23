#include "csapp.h"

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

		if (checkKey(&rio, secretKey) == 0) {

			requestType = getRequest(&rio);

			if (requestType == 0) {
				printf("Request Type = get\n");
				status = retrieveRequest(&rio, connfd);
			}
			else if (requestType == 1) {
				printf("Request Type = put\n");
				status = storeRequest(&rio, connfd);
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

		if (status == 0) {
			printf("Operation Status = success");
		}
		else {
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