/***************************

file: mclist.c

RUDY BROOKS

******************************/

#include "LibCalls.c"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char *argv[]){

  int list_len;
  char *list_buf;

  char *machine_name = argv[1];
  unsigned int tcp_port     = atoi(argv[2]);
  unsigned int secret_key   = atoi(argv[3]);  

  int status = mycloud_listfiles(machine_name, tcp_port, secret_key, &list_buf, &list_len);

  if(status == 0) {
    int i;
    char fileName[FILE_NAME_SIZE];
    for(i = 0; i < list_len; i += FILE_NAME_SIZE) {
      memcpy(&fileName, list_buf + i, FILE_NAME_SIZE);
      printf("%s\n", fileName);
    }
  }

  free(list_buf);
  return status;
}
