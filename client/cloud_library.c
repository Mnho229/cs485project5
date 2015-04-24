#include "csapp.h"
#define NAME_SIZE 80
#define FILE_SIZE 100000
#define REQUEST_SIZE 4
#define DNS_SIZE 40

int mycloud_putfile(char *MachineName, int TCPport, int SecretKey, char *FileName, char *data, int datalen) 
{

  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  // Sets the size of the message. 
  messageSize = 4 + REQUEST_SIZE + NAME_SIZE + 4 + datalen;

  // Allocates memory to the message using the message 
  message = (char*) malloc (sizeof(char)*messageSize);
  // if the message is null meaning empty then it reports a memory error and returns -1. 
  if(message == NULL) 
  { 
    fprintf(stderr, "Memory Error - mcputs\n"); 
    return -1; 
  }
  //This duplicates the message into buffer. 
  char *messageBuf = message;

  // This puts the secret key into the messagebuf my copying the memory. 
  netOrder = htonl(SecretKey);
  memcpy(messageBuf, &netOrder, 4);
  messageBuf += 4;

  // This adds the request into the messageBuf.
  unsigned int request = 1;
  netOrder = htonl(request);
  memcpy(messageBuf, &netOrder, REQUEST_SIZE);
  messageBuf += REQUEST_SIZE;

  // CAdds the filename to the messageBuf
  memcpy(messageBuf, FileName, NAME_SIZE);
  messageBuf += NAME_SIZE;

  // Adds the size of the file into the messageBuf 
  netOrder = htonl(datalen);
  memcpy(messageBuf, &netOrder, 4);
  messageBuf += 4;

  // Copy file data into message buffer
  memcpy(messageBuf, data, datalen);
  messageBuf += datalen;

  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, message, messageSize);
  free(message);

  // This checks to make sure that the operation. 
  size_t num;
  char buf[4];
  unsigned int stat;
  rio_t rio;
  Rio_readinitb(&rio, clientfd);
  
  stat = -1;
  if((num = Rio_readnb(&rio, buf, 4)) == 4) {
    // Copy binary data from buffer
    memcpy(&netOrder, &buf, 4);
    stat = ntohl(netOrder);
  }
  Close(clientfd);
  return stat; // Returns the status for checking in the server. 
}

int mycloud_getfile(char *MachineName, int TCPport, int SecretKey, char *Filename, char **data, int *datalen) 
{

  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  messageSize = 4 + REQUEST_SIZE + NAME_SIZE;

  // Allocates memory to the message using the message 
  message = (char*) malloc (sizeof(char)*messageSize);

  // if the message is null meaning empty then it reports a memory error and returns -1. 
  if(message == NULL) 
    { 
        fprintf(stderr, "Memory Error ~ mcget\n"); 
        return -1; 
    }
  char *messageBuf = message;

  // Adds the secret key into the messagebuf my copying the memory. 
  netOrder = htonl(SecretKey);
  memcpy(messageBuf, &netOrder, 4);
  messageBuf += 4;

  // This adds the request type ie get into the messageBuf.
  unsigned int request = 0;
  netOrder = htonl(request);
  memcpy(messageBuf, &netOrder, REQUEST_SIZE);
  messageBuf += REQUEST_SIZE;

  // Adds the filename into the messageBuf
  memcpy(messageBuf, Filename, NAME_SIZE);
  messageBuf += NAME_SIZE;

  // Opens a connection to the server. 
  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, message, messageSize);
  free(message);

  
  size_t num;
  int GET_STATUS = 4 + 4 + NAME_SIZE;
  char buf[GET_STATUS];
  char fileSizeBuf[4];
  char dataBuf[NAME_SIZE];
  unsigned int stat, bytesInFile;
  
  rio_t rio;
  Rio_readinitb(&rio, clientfd);


  if((num = Rio_readnb(&rio, buf, 4)) == 4) 
  {
    // Copies the bit data into the buffer. 
    memcpy(&netOrder, &buf, 4);
    stat = ntohl(netOrder);
  //  printf("Status: %d\n", stat);
  } 
  else 
  {
    stat = -1;
    //printf("stat read failed\n");
  }


  if((num = Rio_readnb(&rio, fileSizeBuf, 4)) == 4) 
  {
    memcpy(&netOrder, &fileSizeBuf, 4);
    *datalen = htonl(netOrder);
    //printf("bytes in file dl: %d \n", *datalen);
  } 
  else 
  {
    stat = -1;
    //printf("max bytes failed\n");
  }

  //need to do this in order to Rio_readnb to work
  bytesInFile = *datalen;
  
  if((num = Rio_readnb(&rio, dataBuf, bytesInFile)) == bytesInFile) 
  {

    *data = (char*) malloc (sizeof(char)*bytesInFile);
    
    if (*data == NULL) 
      {
        fprintf(stderr, "Memory Error - mcgets \n"); 
        return -1;
      }
    memcpy(*data, &dataBuf, bytesInFile);
  
    //printf("client received %d bytes \n", (int)n);
  } 
  else 
  {
    stat = -1;
    //printf("could not retrieve file data\n");
  }

  Close(clientfd); // Closes the open server. 

  return stat;
}
int mycloud_delfile(char *MachineName, int TCPport, int SecretKey, char *Filename) 
{ 

  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  messageSize = 4 + REQUEST_SIZE + NAME_SIZE;

  // Allocate memory for the message buffer defined by the protocol
  message = (char*) malloc (sizeof(char)*messageSize);
  if(message == NULL) 
    { 
      fprintf(stderr, "Memory Error ~ mcdel\n");
      return -1;
     }
  char *messageBuf = message;

  // Copies the secret key into message buffer
  netOrder = htonl(SecretKey);
  memcpy(messageBuf, &netOrder, 4);
  messageBuf += 4;

  // Copies the request type into message buffer
  unsigned int request = 2;
  netOrder = htonl(request);
  memcpy(messageBuf, &netOrder, REQUEST_SIZE);
  messageBuf += REQUEST_SIZE;

  // Copies the file name into message buffer
  memcpy(messageBuf, Filename, NAME_SIZE);
  messageBuf += NAME_SIZE;

  // opens a connection 
  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, message, messageSize);
  //Close(clientfd);
  free(message);

  //checks status of the operation. 
  size_t num;
  char buf[4];
  unsigned int stat;
  rio_t rio;
  Rio_readinitb(&rio, clientfd);
  
  stat = -1;
  if((num = Rio_readnb(&rio, buf, 4)) == 4) 
  {
    // Copy binary data from buffer
    memcpy(&netOrder, &buf, 4);
    stat = ntohl(netOrder);
  }
  Close(clientfd);
  return stat;

}
int mycloud_listfiles(char *MachineName, int TCPport, int SecretKey, char **list, unsigned int *list_len) 
{
  int clientfd;
  char *message;
  unsigned int messageSize, netOrder;

  messageSize = 4 + REQUEST_SIZE;


  message = (char*) malloc (sizeof(char)*messageSize);
  if(message == NULL) 
    { 
      fprintf(stderr, "Memory Error - mclist\n"); 
      return -1;
     }
  char *messageBuf = message;


  netOrder = htonl(SecretKey);
  memcpy(messageBuf, &netOrder, 4);
  messageBuf += 4;

  unsigned int request = 3;
  netOrder = htonl(request);
  memcpy(messageBuf, &netOrder, REQUEST_SIZE);
  messageBuf += REQUEST_SIZE;

  // Opens a connection then writes to the server. 
  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, message, messageSize);
  free(message);

 
  size_t num;
  char statBuf[4];
  char listSizeBuf[4];
  char listBuf[FILE_SIZE];
  unsigned int stat;
  rio_t rio;
  Rio_readinitb(&rio, clientfd);

  // Get the stat
  if((num = Rio_readnb(&rio, statBuf, 4)) == 4) 
  {
   
    memcpy(&netOrder, &statBuf, 4);
    stat = ntohl(netOrder);
  } 
  else 
  {
    stat = -1;
  }

  // Get the list size
  if((num = Rio_readnb(&rio, listSizeBuf, 4)) == 4) 
  {
    // Copy binary data from buffer
    memcpy(&netOrder, &listSizeBuf, 4);
    *list_len = ntohl(netOrder);
  } 
  else 
  {
    stat = -1;
  }
  // Get the data
  if((num = Rio_readnb(&rio, listBuf, *list_len)) == *list_len) 
  {
    *list = (char*) malloc (*list_len);
    if(*list == NULL) { fprintf(stderr, "Memory Error\n"); return -1; }

 
    memcpy(*list, &listBuf, *list_len);
    stat = 0;
  } 

  else 
  {
    stat = -1;
  }

  Close(clientfd);
  return stat;
}
