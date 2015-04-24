#include "csapp.h"
#define NAME_SIZE 80
#define FILE_SIZE 100000
#define REQUEST_SIZE 4
#define DNS_SIZE 40

int mycloud_putfile(char *MachineName, int TCPport, int SecretKey, char *FileName, char *data, int datalen) 
{

  int clientfd;
  char *reply;
  unsigned int replySize, netOrder;

  // Sets the size of the reply. 
  replySize = 4 + REQUEST_SIZE + NAME_SIZE + 4 + datalen;

  // Allocates memory to the reply using the reply 
  reply = (char*) malloc (sizeof(char)*replySize);
  // if the reply is null meaning empty then it reports a memory error and returns -1. 
  if(reply == NULL) 
  { 
    fprintf(stderr, "Memory Error - mcputs\n"); 
    return -1; 
  }
  //This duplicates the reply into buffer. 
  char *replyBuf = reply;

  // This puts the secret key into the replyBuf my copying the memory. 
  netOrder = htonl(SecretKey);
  memcpy(replyBuf, &netOrder, 4);
  replyBuf += 4;

  // This adds the request into the replyBuf.
  unsigned int request = 1;
  netOrder = htonl(request);
  memcpy(replyBuf, &netOrder, REQUEST_SIZE);
  replyBuf += REQUEST_SIZE;

  // CAdds the filename to the replyBuf
  memcpy(replyBuf, FileName, NAME_SIZE);
  replyBuf += NAME_SIZE;

  // Adds the size of the file into the replyBuf 
  netOrder = htonl(datalen);
  memcpy(replyBuf, &netOrder, 4);
  replyBuf += 4;

  // Copy file data into reply buffer
  memcpy(replyBuf, data, datalen);
  replyBuf += datalen;

  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, reply, replySize);
  free(reply);

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
  char *reply;
  unsigned int replySize, netOrder;

  replySize = 4 + REQUEST_SIZE + NAME_SIZE;

  // Allocates memory to the reply using the reply 
  reply = (char*) malloc (sizeof(char)*replySize);

  // if the reply is null meaning empty then it reports a memory error and returns -1. 
  if(reply == NULL) 
    { 
        fprintf(stderr, "Memory Error ~ mcget\n"); 
        return -1; 
    }
  char *replyBuf = reply;

  // Adds the secret key into the replyBuf my copying the memory. 
  netOrder = htonl(SecretKey);
  memcpy(replyBuf, &netOrder, 4);
  replyBuf += 4;

  // This adds the request type ie get into the replyBuf.
  unsigned int request = 0;
  netOrder = htonl(request);
  memcpy(replyBuf, &netOrder, REQUEST_SIZE);
  replyBuf += REQUEST_SIZE;

  // Adds the filename into the replyBuf
  memcpy(replyBuf, Filename, NAME_SIZE);
  replyBuf += NAME_SIZE;

  // Opens a connection to the server. 
  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, reply, replySize);
  free(reply);

  
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
  char *reply;
  unsigned int replySize, netOrder;

  replySize = 4 + REQUEST_SIZE + NAME_SIZE;

  // Allocate memory for the reply buffer defined by the protocol
  reply = (char*) malloc (sizeof(char)*replySize);
  if(reply == NULL) 
    { 
      fprintf(stderr, "Memory Error ~ mcdel\n");
      return -1;
     }
  char *replyBuf = reply;

  // Copies the secret key into reply buffer
  netOrder = htonl(SecretKey);
  memcpy(replyBuf, &netOrder, 4);
  replyBuf += 4;

  // Copies the request type into reply buffer
  unsigned int request = 2;
  netOrder = htonl(request);
  memcpy(replyBuf, &netOrder, REQUEST_SIZE);
  replyBuf += REQUEST_SIZE;

  // Copies the file name into reply buffer
  memcpy(replyBuf, Filename, NAME_SIZE);
  replyBuf += NAME_SIZE;

  // opens a connection 
  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, reply, replySize);
  //Close(clientfd);
  free(reply);

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
int mycloud_listfiles(char *MachineName, int TCPport, int SecretKey, char **list, unsigned int *Buf_Len) 
{
  int clientfd;
  char *reply;
  unsigned int replySize, netOrder;

  replySize = 4 + REQUEST_SIZE;


  reply = (char*) malloc (sizeof(char)*replySize);
  if(reply == NULL) 
    {

      fprintf(stderr, "Memory Error - mclist\n"); 
      return -1;
    
    }
  char *replyBuf = reply;


  netOrder = htonl(SecretKey);
  memcpy(replyBuf, &netOrder, 4);
  replyBuf += 4;

  unsigned int request = 3;
  netOrder = htonl(request);
  memcpy(replyBuf, &netOrder, REQUEST_SIZE);
  replyBuf += REQUEST_SIZE;

  // Opens a connection then writes to the server. 
  clientfd = Open_clientfd(MachineName, TCPport);
  Rio_writen(clientfd, reply, replySize);
  free(reply);

 
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
    *Buf_Len = ntohl(netOrder);

  } 
  
  else 
  {
    stat = -1;
  }
  // Get the data
  if((num = Rio_readnb(&rio, listBuf, *Buf_Len)) == *Buf_Len) 
  {
    
    *list = (char*) malloc (*Buf_Len);
    if(*list == NULL) { fprintf(stderr, "Memory Error\n"); return -1; }
    memcpy(*list, &listBuf, *Buf_Len);
    stat = 0;

  } 

  else 
  {
    stat = -1;
  }

  Close(clientfd);
  return stat;
}
