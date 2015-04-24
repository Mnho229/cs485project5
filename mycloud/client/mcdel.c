/****************************************************

file: mydel.c

RUDY BROOKS

*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "LibCalls.c"

int main(int argc, char *argv[]) {

  char *machine_name      = argv[1];
  unsigned int tcp_port   = atoi(argv[2]);
  unsigned int secret_key = atoi(argv[3]);   
  char *file_name         = argv[4];


  //run Libary/API call
  int status = mycloud_delfile(machine_name, tcp_port, secret_key, file_name);
  //printf("Status = %d", status);
  return status;

  /******DEBUGGING********************
  int i;
  for(i=1;i<5;++i)
    printf("%d : %s \n", i, argv[i]);
  ************************************/
}
