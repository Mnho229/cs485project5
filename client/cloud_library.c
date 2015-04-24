#include "csapp.h"
#define NAME_SIZE 80
#define FILE_SIZE 100000
#define REQUEST_SIZE 4
#define DNS_SIZE 40

int mycloud_putfile(char *MachineName, int TCPport, int SecretKey, char *FileName, char *data, int datalen) {
  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  // Set message size according to the protocol
  messageSize = 4 + REQUEST_SIZE + NAME_SIZE + 4 + datalen;

  // Allocate memory for the message buffer defined by the protocol
  message = (char*) malloc (sizeof(char)*messageSize);
  if(message == NULL) { fprintf(stderr, "Memory Error - mcputs\n"); return -1; }
  char *messagePtr = message;

  // Copy secret key into message buffer
  netOrder = htonl(SecretKey);
  memcpy(messagePtr, &netOrder, 4);
  messagePtr += 4;

  // Copy request type into message buffer
  unsigned int request = 1;
  netOrder = htonl(request);
  memcpy(messagePtr, &netOrder, REQUEST_SIZE);
  messagePtr += REQUEST_SIZE;

  // Copy file name into message buffer
  memcpy(messagePtr, FileName, NAME_SIZE);
  messagePtr += NAME_SIZE;

  // Copy file size into message buffer
  netOrder = htonl(datalen);
  memcpy(messagePtr, &netOrder, 4);
  messagePtr += 4;

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
  char buf[4];
  unsigned int status;
  rio_t rio;
  Rio_readinitb(&rio, clientfd);
  
  status = -1;
  if((n = Rio_readnb(&rio, buf, 4)) == 4) {
    // Copy binary data from buffer
    memcpy(&netOrder, &buf, 4);
    status = ntohl(netOrder);
  }
  Close(clientfd);
  return status;
}

int mycloud_getfile(char *MachineName, int TCPport, int SecretKey, char *Filename, char **data, int *datalen) {

  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  messageSize = 4 + REQUEST_SIZE + NAME_SIZE;

  // Allocate memory for the message buffer defined by the protocol
  message = (char*) malloc (sizeof(char)*messageSize);
  if(message == NULL) { fprintf(stderr, "Memory Error - mcputs\n"); return -1; }
  char *messagePtr = message;

  // Copy secret key into message buffer
  netOrder = htonl(SecretKey);
  memcpy(messagePtr, &netOrder, 4);
  messagePtr += 4;

  // Copy request type into message buffer
  unsigned int request = 0;
  netOrder = htonl(request);
  memcpy(messagePtr, &netOrder, REQUEST_SIZE);
  messagePtr += REQUEST_SIZE;

  // Copy file name into message buffer
  memcpy(messagePtr, Filename, NAME_SIZE);
  messagePtr += NAME_SIZE;

  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, message, messageSize);
  free(message);

  //
  // Receieve operational status
  //
  
  size_t n;
  int GET_STATUS = 4 + 4 + NAME_SIZE;
  char buf[GET_STATUS];
  char fileSizeBuf[4];
  char dataBuf[NAME_SIZE];
  unsigned int status, bytesInFile;
  
  rio_t rio;
  Rio_readinitb(&rio, clientfd);


  if((n = Rio_readnb(&rio, buf, 4)) == 4) {
    // Copy binary data from buffer
    memcpy(&netOrder, &buf, 4);
    status = ntohl(netOrder);
  //  printf("Status: %d\n", status);
  } else {
    status = -1;
    //printf("status read failed\n");
  }


  if((n = Rio_readnb(&rio, fileSizeBuf, 4)) == 4) {
    memcpy(&netOrder, &fileSizeBuf, 4);
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
int mycloud_delfile(char *MachineName, int TCPport, int SecretKey, char *Filename) { 

  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  messageSize = 4 + REQUEST_SIZE + NAME_SIZE;

  // Allocate memory for the message buffer defined by the protocol
  message = (char*) malloc (sizeof(char)*messageSize);
  if(message == NULL) { fprintf(stderr, "Memory Error - mcputs\n"); return -1; }
  char *messagePtr = message;

  // Copy secret key into message buffer
  netOrder = htonl(SecretKey);
  memcpy(messagePtr, &netOrder, 4);
  messagePtr += 4;

  // Copy request type into message buffer
  unsigned int request = 2;
  netOrder = htonl(request);
  memcpy(messagePtr, &netOrder, REQUEST_SIZE);
  messagePtr += REQUEST_SIZE;

  // Copy file name into message buffer
  memcpy(messagePtr, Filename, NAME_SIZE);
  messagePtr += NAME_SIZE;

  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, message, messageSize);
  //Close(clientfd);
  free(message);

  //
  // Receieve operational status
  //
  size_t n;
  char buf[4];
  unsigned int status;
  rio_t rio;
  Rio_readinitb(&rio, clientfd);
  
  status = -1;
  if((n = Rio_readnb(&rio, buf, 4)) == 4) {
    // Copy binary data from buffer
    memcpy(&netOrder, &buf, 4);
    status = ntohl(netOrder);
  }
  Close(clientfd);
  return status;

}
int mycloud_listfiles(char *MachineName, int TCPport, int SecretKey, char **list, int *list_len) {

  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  messageSize = 4 + REQUEST_SIZE;

  // Allocate memory for the message buffer defined by the protocol
  message = (char*) malloc (sizeof(char)*messageSize);
  if(message == NULL) { fprintf(stderr, "Memory Error - mclist\n"); return -1; }
  char *messagePtr = message;

  // Copy secret key into message buffer
  netOrder = htonl(SecretKey);
  memcpy(messagePtr, &netOrder, 4);
  messagePtr += 4;

  // Copy request type into message buffer
  unsigned int request = 3;
  netOrder = htonl(request);
  memcpy(messagePtr, &netOrder, REQUEST_SIZE);
  messagePtr += REQUEST_SIZE;

  // Write to server
  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, message, messageSize);
  free(message);

  //
  // Receieve operational status
  //
  size_t n;
  char statusBuf[4];
  char listSizeBuf[4];
  char listBuf[NAME_SIZE];
  unsigned int status;
  rio_t rio;
  Rio_readinitb(&rio, clientfd);

  // Get the status
  if((n = Rio_readnb(&rio, statusBuf, 4)) == 4) {
    // Copy binary data from buffer
    memcpy(&netOrder, &statusBuf, 4);
    status = ntohl(netOrder);
  } else {
    status = -1;
  }

  // Get the list size
  if((n = Rio_readnb(&rio, listSizeBuf, 4)) == 4) {
    // Copy binary data from buffer
    memcpy(&netOrder, &listSizeBuf, 4);
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
