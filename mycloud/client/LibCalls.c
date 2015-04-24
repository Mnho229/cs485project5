/**************************

file: LibCalls.c

RUDY BROOKS

***************************/

#include "../csapp.h"
#include "../mycloud.h"


// Send bytes of data to the cloud server
// Return 0 for success, -1 for error
int mycloud_putfile(char *MachineName, int TCPport, int SecretKey, char *FileName, char *data, int datalen) {
  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  // Set message size according to the protocol
  messageSize = SECRET_KEY_SIZE + REQUEST_TYPE_SIZE + FILE_NAME_SIZE + MAX_NUM_BYTES_IN_FILE + datalen;

  // Allocate memory for the message buffer defined by the protocol
  message = (char*) malloc (sizeof(char)*messageSize);
  if(message == NULL) { fprintf(stderr, "Memory Error - mcputs\n"); return -1; }
  char *messagePtr = message;

  // Copy secret key into message buffer
  netOrder = htonl(SecretKey);
  memcpy(messagePtr, &netOrder, SECRET_KEY_SIZE);
  messagePtr += SECRET_KEY_SIZE;

  // Copy request type into message buffer
  unsigned int request = 1;
  netOrder = htonl(request);
  memcpy(messagePtr, &netOrder, REQUEST_TYPE_SIZE);
  messagePtr += REQUEST_TYPE_SIZE;

  // Copy file name into message buffer
  memcpy(messagePtr, FileName, FILE_NAME_SIZE);
  messagePtr += FILE_NAME_SIZE;

  // Copy file size into message buffer
  netOrder = htonl(datalen);
  memcpy(messagePtr, &netOrder, MAX_NUM_BYTES_IN_FILE);
  messagePtr += MAX_NUM_BYTES_IN_FILE;

  // Copy file data into message buffer
  memcpy(messagePtr, data, datalen);
  messagePtr += datalen;

  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, message, messageSize);
  free(message);

  //
  // Receieve operational status
  //
  size_t n;
  char buf[STATUS_SIZE];
  unsigned int status;
  rio_t rio;
  Rio_readinitb(&rio, clientfd);
  
  status = -1;
  if((n = Rio_readnb(&rio, buf, STATUS_SIZE)) == STATUS_SIZE) {
    // Copy binary data from buffer
    memcpy(&netOrder, &buf, STATUS_SIZE);
    status = ntohl(netOrder);
  }
  Close(clientfd);
  return status;
}

//retrieve bytes of data from "FileName" 
//returns the number of bytes in the file, or -1 for error
int mycloud_getfile(char *MachineName, int TCPport, int SecretKey, char *Filename, char **data, int *datalen) {

  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  messageSize = SECRET_KEY_SIZE + REQUEST_TYPE_SIZE + FILE_NAME_SIZE;

  // Allocate memory for the message buffer defined by the protocol
  message = (char*) malloc (sizeof(char)*messageSize);
  if(message == NULL) { fprintf(stderr, "Memory Error - mcputs\n"); return -1; }
  char *messagePtr = message;

  // Copy secret key into message buffer
  netOrder = htonl(SecretKey);
  memcpy(messagePtr, &netOrder, SECRET_KEY_SIZE);
  messagePtr += SECRET_KEY_SIZE;

  // Copy request type into message buffer
  unsigned int request = 0;
  netOrder = htonl(request);
  memcpy(messagePtr, &netOrder, REQUEST_TYPE_SIZE);
  messagePtr += REQUEST_TYPE_SIZE;

  // Copy file name into message buffer
  memcpy(messagePtr, Filename, FILE_NAME_SIZE);
  messagePtr += FILE_NAME_SIZE;

  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, message, messageSize);
  free(message);

  //
  // Receieve operational status
  //
  
  size_t n;
  int GET_STATUS_SIZE = STATUS_SIZE + MAX_NUM_BYTES_IN_FILE + MAX_FILE_SIZE;
  char buf[GET_STATUS_SIZE];
  char fileSizeBuf[MAX_NUM_BYTES_IN_FILE];
  char dataBuf[MAX_FILE_SIZE];
  unsigned int status, bytesInFile;
  
  rio_t rio;
  Rio_readinitb(&rio, clientfd);


  if((n = Rio_readnb(&rio, buf, STATUS_SIZE)) == STATUS_SIZE) {
    // Copy binary data from buffer
    memcpy(&netOrder, &buf, STATUS_SIZE);
    status = ntohl(netOrder);
  //  printf("Status: %d\n", status);
  } else {
    status = -1;
    //printf("status read failed\n");
  }


  if((n = Rio_readnb(&rio, fileSizeBuf, MAX_NUM_BYTES_IN_FILE)) == MAX_NUM_BYTES_IN_FILE) {
    memcpy(&netOrder, &fileSizeBuf, MAX_NUM_BYTES_IN_FILE);
    *datalen = htonl(netOrder);
    //printf("bytes in file dl: %d \n", *datalen);
  } else {
    status = -1;
    //printf("max bytes failed\n");
  }

  //need to do this in order to Rio_readnb to work
  bytesInFile = *datalen;
  
  if((n = Rio_readnb(&rio, dataBuf, bytesInFile)) == bytesInFile) {

    *data = (char*) malloc (sizeof(char)*bytesInFile);
    if (*data == NULL) {fprintf(stderr, "Memory Error - mcgets \n"); return -1;}
    memcpy(*data, &dataBuf, bytesInFile);
  
    //printf("client received %d bytes \n", (int)n);
  } else {
    status = -1;
    //printf("could not retrieve file data\n");
  }

  Close(clientfd);

  return status;
}



// send message to server requesting to delete "FileName"
// return 0 for success, -1 for error
int mycloud_delfile(char *MachineName, int TCPport, int SecretKey, char *Filename) { 

  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  messageSize = SECRET_KEY_SIZE + REQUEST_TYPE_SIZE + FILE_NAME_SIZE;

  // Allocate memory for the message buffer defined by the protocol
  message = (char*) malloc (sizeof(char)*messageSize);
  if(message == NULL) { fprintf(stderr, "Memory Error - mcputs\n"); return -1; }
  char *messagePtr = message;

  // Copy secret key into message buffer
  netOrder = htonl(SecretKey);
  memcpy(messagePtr, &netOrder, SECRET_KEY_SIZE);
  messagePtr += SECRET_KEY_SIZE;

  // Copy request type into message buffer
  unsigned int request = 2;
  netOrder = htonl(request);
  memcpy(messagePtr, &netOrder, REQUEST_TYPE_SIZE);
  messagePtr += REQUEST_TYPE_SIZE;

  // Copy file name into message buffer
  memcpy(messagePtr, Filename, FILE_NAME_SIZE);
  messagePtr += FILE_NAME_SIZE;

  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, message, messageSize);
  //Close(clientfd);
  free(message);

  //
  // Receieve operational status
  //
  size_t n;
  char buf[STATUS_SIZE];
  unsigned int status;
  rio_t rio;
  Rio_readinitb(&rio, clientfd);
  
  status = -1;
  if((n = Rio_readnb(&rio, buf, STATUS_SIZE)) == STATUS_SIZE) {
    // Copy binary data from buffer
    memcpy(&netOrder, &buf, STATUS_SIZE);
    status = ntohl(netOrder);
  }
  Close(clientfd);
  return status;

}

// sends request to server for all files in server
// server - prints each file separated by '\n'
// 0 = success, -1 = error

int mycloud_listfiles(char *MachineName, int TCPport, int SecretKey, char **list, int *list_len) {

  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  messageSize = SECRET_KEY_SIZE + REQUEST_TYPE_SIZE;

  // Allocate memory for the message buffer defined by the protocol
  message = (char*) malloc (sizeof(char)*messageSize);
  if(message == NULL) { fprintf(stderr, "Memory Error - mclist\n"); return -1; }
  char *messagePtr = message;

  // Copy secret key into message buffer
  netOrder = htonl(SecretKey);
  memcpy(messagePtr, &netOrder, SECRET_KEY_SIZE);
  messagePtr += SECRET_KEY_SIZE;

  // Copy request type into message buffer
  unsigned int request = 3;
  netOrder = htonl(request);
  memcpy(messagePtr, &netOrder, REQUEST_TYPE_SIZE);
  messagePtr += REQUEST_TYPE_SIZE;

  // Write to server
  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, message, messageSize);
  free(message);

  //
  // Receieve operational status
  //
  size_t n;
  char statusBuf[STATUS_SIZE];
  char listSizeBuf[MAX_NUM_BYTES_IN_FILE];
  char listBuf[MAX_FILE_SIZE];
  unsigned int status;
  rio_t rio;
  Rio_readinitb(&rio, clientfd);

  // Get the status
  if((n = Rio_readnb(&rio, statusBuf, STATUS_SIZE)) == STATUS_SIZE) {
    // Copy binary data from buffer
    memcpy(&netOrder, &statusBuf, STATUS_SIZE);
    status = ntohl(netOrder);
  } else {
    status = -1;
  }

  // Get the list size
  if((n = Rio_readnb(&rio, listSizeBuf, MAX_NUM_BYTES_IN_FILE)) == MAX_NUM_BYTES_IN_FILE) {
    // Copy binary data from buffer
    memcpy(&netOrder, &listSizeBuf, MAX_NUM_BYTES_IN_FILE);
    *list_len = ntohl(netOrder);
  } else {
    status = -1;
  }

  // Get the data
  if((n = Rio_readnb(&rio, listBuf, *list_len)) == *list_len) {
    // Allocate memory for the data
    *list = (char*) malloc (*list_len);
    if(*list == NULL) { fprintf(stderr, "Memory Error\n"); return -1; }

    // Copy binary data from buffer
    memcpy(*list, &listBuf, *list_len);
    status = 0;
  } else {
    status = -1;
  }

  Close(clientfd);
  return status;
}

