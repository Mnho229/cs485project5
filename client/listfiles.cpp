extern "C" {
   #include "csapp.h"
   #include "cloud_library.c"
}
#include <iostream>
#include <stdlib.h>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) 
{
    int port, key;
    unsigned int list_length;
    char *list_buff;
    char *host;

    if (argc != 4) {
		fprintf(stderr, "usage: %s <host> <port> <key> \n", argv[0]);
		exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);
    key = atoi(argv[3]);

    int request = mycloud_listfiles(host, port, key, &list_buff, &list_length);

    if (request == 0) {
        char fileName[80];
        for (int i = 0; i < list_length ; i+=80) {
            memcpy(&fileName, list_buff + i, 80);
            printf("%s\n", fileName);
        }
    }
    exit(0);
}