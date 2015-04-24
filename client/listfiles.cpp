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
    int clientfd, port, key, list_length;
    char *list_buff;
    char *host;
    rio_t rio;

    if (argc != 4) {
		fprintf(stderr, "usage: %s <host> <port> <key> \n", argv[0]);
		exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);
    key = atoi(argv[3]);

    int request = mycloud_listfiles(host, port, key, &list_buff, &list_length);

    if (request == 0) {
        cout << list_buff << endl;
    }
    exit(0);
}