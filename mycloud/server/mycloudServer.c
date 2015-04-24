/***********************

file: mycloudServer.c

RUDY BROOKS

************************/

#include <stdio.h>
#include <stdlib.h>
#include "../mycloud.h"
#include "../csapp.h"
#include <string.h>

// Globals
char fileList[MAX_NUM_FILES][FILE_NAME_SIZE];
unsigned int numFiles = 0;

// Prototypes
int validKey(rio_t *rio, unsigned int secretKey);
int getRequest(rio_t *rio);
int storeRequest(rio_t *rio, int connfd);
int retrieveRequest(rio_t *rio, int connfd);
int deleteRequest(rio_t *rio, int connfd);
int listFilesRequest(rio_t *rio, int connfd);
int addFileToList(char *fileName);
int removeFileFromList(char *fileName);
int fileInList(char *fileName);

int main(int argc, char **argv) 
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;
    unsigned int secretKey;

    if (argc != 3) {
	fprintf(stderr, "usage: %s <port> <secret key>\n", argv[0]);
	exit(0);
    }
    port = atoi(argv[1]);
    secretKey = atoi(argv[2]);

    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

	/* Determine the domain name and IP address of the client */
	hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			   sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	haddrp = inet_ntoa(clientaddr.sin_addr);
	printf("server connected to %s (%s)\n", hp->h_name, haddrp);

        int requestType = -1;
        int status = -1;
        rio_t rio;
        Rio_readinitb(&rio, connfd);
        
        // Authenticate key
        if(validKey(&rio, secretKey) == 0) {
            // Identify and execute request type
            requestType = getRequest(&rio);
            if(requestType == 0) {
                printf("Request Type     = get\n");
                status = retrieveRequest(&rio, connfd);
            } else if(requestType == 1) {
                printf("Request Type     = put\n");
                status = storeRequest(&rio, connfd);
            } else if(requestType == 2) {
                printf("Request Type     = del\n");
                status = deleteRequest(&rio, connfd);
            } else if(requestType == 3) {
                printf("Request Type     = list\n");
                status = listFilesRequest(&rio, connfd);
            } else {
                printf("Request Type     = invalid(%d)\n", requestType);
            }
        }

        if(status == 0) { printf("Operation Status = success\n"); }
        else { printf("Operation Status = error\n"); }
        printf("------------------------------------\n");
	Close(connfd);
    }
    exit(0);
}

// Return 0 if key is valid, -1 if invalid
int validKey(rio_t *rio, unsigned int secretKey) {
    size_t n;
    char buf[SECRET_KEY_SIZE];
    unsigned int clientKey, netOrder;

    // Read binary data into buffer
    if((n = Rio_readnb(rio, buf, SECRET_KEY_SIZE)) == SECRET_KEY_SIZE) {
        // Copy binary data from buffer
        memcpy(&netOrder, &buf, SECRET_KEY_SIZE);
        clientKey = ntohl(netOrder);        

        printf("Secret Key       = %d\n", clientKey);
        if(clientKey == secretKey) { return 0; } 
        else { return -1; }
    }    
    return -1;
}

// Returns the request type or -1 if invalid
int getRequest(rio_t *rio) {
    size_t n;
    char buf[REQUEST_TYPE_SIZE];
    unsigned int requestType, netOrder;

    // Read binary data into buffer
    if((n = Rio_readnb(rio, buf, REQUEST_TYPE_SIZE)) == REQUEST_TYPE_SIZE) {
        // Copy binary data from buffer
        memcpy(&netOrder, &buf, REQUEST_TYPE_SIZE);
        requestType = ntohl(netOrder);

        return requestType;
    }
    return -1;
}

// Return 0 if the store is successful, -1 if fails
int storeRequest(rio_t *rio, int connfd) {
    size_t n;
    char fileNameBuf[FILE_NAME_SIZE];
    char fileName[FILE_NAME_SIZE];
    char fileSizeBuf[MAX_NUM_BYTES_IN_FILE];
    unsigned int fileSize, netOrder, status, messageSize;
    char dataBuf[MAX_FILE_SIZE];
    char *data, *message;
    FILE *file;

    /******************************
     * Store Request Message      *
     ******************************/
    
    // Read file name
    if((n = Rio_readnb(rio, fileNameBuf, FILE_NAME_SIZE)) == FILE_NAME_SIZE) {
        // Copy binary data from buffer
        memcpy(&fileName, &fileNameBuf, FILE_NAME_SIZE);
        printf("Filename         = %s\n", fileName);
    } else {
        printf("Filename         = NONE\n");
        status = -1;
    }

    // Read file size
    if((n = Rio_readnb(rio, fileSizeBuf, MAX_NUM_BYTES_IN_FILE)) == MAX_NUM_BYTES_IN_FILE) {
        // Copy binary data from buffer
        memcpy(&netOrder, &fileSizeBuf, MAX_NUM_BYTES_IN_FILE);
        fileSize = ntohl(netOrder);
    } else {
        status = -1;
    }

    // Read file data
    if((n = Rio_readnb(rio, dataBuf, fileSize)) == fileSize) {
        // Allocate memory for the data
        data = (char*) malloc (sizeof(char)*fileSize);
        if(data == NULL) { fprintf(stderr, "Memory Error\n"); return -1; }

        // Copy binary data from buffer
        memcpy(data, &dataBuf, fileSize);
    } else {
        status = -1;
    }

    // Write to file and update file list
    if((file = Fopen(fileName, "w")) != NULL) {
        Fwrite(data, sizeof(char), fileSize, file);
        Fclose(file);
        if(addFileToList(fileName) == 0) { status = 0; }
        else {status = -1; }
    } else {
        status = -1;
    }
    free(data);

    /******************************
     * Store Response Message     *
     ******************************/

    // Set message size according to the protocol
    messageSize = STATUS_SIZE;

    // Allocate memory for the message buffer defined by the protocol
    message = (char*) malloc (sizeof(char*)*messageSize);
    if(message == NULL) { fprintf(stderr, "Memory Error\n"); return -1; }
    char *messagePtr = message;

    // Copy the operational status into message buffer
    netOrder = htonl(status);
    memcpy(messagePtr, &netOrder, STATUS_SIZE);
    messagePtr += STATUS_SIZE;

    // Send the response message
    Rio_writen(connfd, message, messageSize);
    free(message);

    return status;
}

// Return 0 if the retrieve is successful, -1 if fails
int retrieveRequest(rio_t *rio, int connfd) {
    size_t n;
    char fileNameBuf[FILE_NAME_SIZE];
    char fileName[FILE_NAME_SIZE];
    unsigned int fileSize, netOrder, status, messageSize;
    char *data, *message;
    FILE *file;

    /******************************
     * Retrieve Request Message   *
     ******************************/

    // Read file name
    if((n = Rio_readnb(rio, fileNameBuf, FILE_NAME_SIZE)) == FILE_NAME_SIZE) {
        // Copy binary data from buffer
        memcpy(&fileName, &fileNameBuf, FILE_NAME_SIZE);
        printf("Filename         = %s\n", fileName);
    } else {
        printf("Filename         = NONE\n");
        status = -1;
    }

    /*******************************
     * Retrieve Response Message   *
     *******************************/

    // Check if file is in list
    if(fileInList(fileName) == -1) { fileSize = 0; status = -1; }
    else {
        // Check if file exists and open it
        file = fopen(fileName, "r");
        if(file == 0) { fprintf(stderr, "Cannot open input file\n"); fileSize = 0; status = -1; }
        else {
            // Obtain file size
            fseek(file, 0, SEEK_END);
            fileSize = ftell(file);
            rewind(file);

            // Allocate memory for the data buffer
            data = (char*) malloc (sizeof(char)*fileSize);
            if(data == NULL) { fprintf(stderr, "Memory Error\n"); return -1; }

            // Copy file data into data buffer
            if((n = fread(data, 1, fileSize, file)) == fileSize) { fclose(file); status = 0; }
            else { fileSize = 0; status = -1; }
        }
    }

    // Set message size according to the protocol
    messageSize = STATUS_SIZE + MAX_NUM_BYTES_IN_FILE + fileSize;

    // Allocate memory for the message buffer defined by the protocol
    message = (char*) malloc (sizeof(char*)*messageSize);
    if(message == NULL) { fprintf(stderr, "Memory Error\n"); return -1; }
    char *messagePtr = message;

    // Copy the operational status into message buffer
    netOrder = htonl(status);
    memcpy(messagePtr, &netOrder, STATUS_SIZE);
    messagePtr += STATUS_SIZE;

    // Copy the file size into message buffer
    netOrder = htonl(fileSize);
    memcpy(messagePtr, &netOrder, MAX_NUM_BYTES_IN_FILE);
    messagePtr += MAX_NUM_BYTES_IN_FILE;

    // Copy file data into message buffer
    memcpy(messagePtr, data, fileSize);
    messagePtr += fileSize;
    free(data);

    // Send the response message
    Rio_writen(connfd, message, messageSize);
    free(message);

    return status;
}

// Returns 0 if the delete is successful, -1 if fails
int deleteRequest(rio_t *rio, int connfd) {
    size_t n;
    char fileNameBuf[FILE_NAME_SIZE];
    char fileName[FILE_NAME_SIZE];
    unsigned int netOrder, status, messageSize;
    char *message;   

    /*******************************
     * Delete Request Message      *
     *******************************/
    
    // Read file name
    if((n = Rio_readnb(rio, fileNameBuf, FILE_NAME_SIZE)) == FILE_NAME_SIZE) {
        // Copy binary data from buffer
        memcpy(&fileName, &fileNameBuf, FILE_NAME_SIZE);
        printf("Filename         = %s\n", fileName);
    } else {
        printf("Filename         = NONE\n");
        status = -1;
    }

    /*******************************
     * Delete Response Message     *
     *******************************/

    // Check if file is in list and remove it
    if(removeFileFromList(fileName) == -1) { status = -1; }
    else {
        // Delete file
        if(remove(fileName) != 0) { status = -1; }
        else { status = 0; }
    }

    // Set message size according to the protocol
    messageSize = STATUS_SIZE;

    // Allocate memory for the message buffer defined by the protocol
    message = (char*) malloc (sizeof(char*)*messageSize);
    if(message == NULL) { fprintf(stderr, "Memory Error\n"); return -1; }
    char *messagePtr = message;

    // Copy the operational status into message buffer
    netOrder = htonl(status);
    memcpy(messagePtr, &netOrder, STATUS_SIZE);
    messagePtr += STATUS_SIZE;

    // Send the response message
    Rio_writen(connfd, message, messageSize);
    free(message);

    return status;
}

// Return 0 if the listing is successful, -1 if fails
int listFilesRequest(rio_t *rio, int connfd) {
    unsigned int datalen, netOrder, status, messageSize;
    char *message;

    /*******************************
     * List Files Response Message *
     *******************************/

    // Obtain data length
    datalen = numFiles * FILE_NAME_SIZE;

    // Set message size according to the protocol
    messageSize = STATUS_SIZE + MAX_NUM_BYTES_IN_FILE + datalen;

    // Allocate memory for the message buffer defined by the protocol
    message = (char*) malloc (sizeof(char*)*messageSize);
    if(message == NULL) { fprintf(stderr, "Memory Error\n"); return -1; }
    char *messagePtr = message;
    status = 0;

    // Copy the operational status into message buffer
    netOrder = htonl(status);
    memcpy(messagePtr, &netOrder, STATUS_SIZE);
    messagePtr += STATUS_SIZE;

    // Copy the data length into message buffer
    netOrder = htonl(datalen);
    memcpy(messagePtr, &netOrder, MAX_NUM_BYTES_IN_FILE);
    messagePtr += MAX_NUM_BYTES_IN_FILE;

    // Copy file data into message buffer
    memcpy(messagePtr, fileList, datalen);
    messagePtr += datalen;

    // Send the response message
    Rio_writen(connfd, message, messageSize);
    free(message);

    return status;
}

// Adds the file name to the last element of the file list
// Returns 0 if the file is added to the list, -1 if not added (already in list or list full)
int addFileToList(char *fileName) {
    if((fileInList(fileName)) == -1 && (numFiles < MAX_NUM_FILES)) {
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
    if(((i = fileInList(fileName)) != -1) && (i < MAX_NUM_FILES - 1)) {
        // Overwrite file name and move all remaining elements
        memmove(fileList[i], fileList[i+1], numFiles * FILE_NAME_SIZE);
        numFiles--;
        return 0;
    } else if(i == MAX_NUM_FILES - 1) {
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

