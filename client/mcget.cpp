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
    int port, key, datalen;
    char *host, *filename;
    char *data = new char[10000];


    if (argc != 5) {
		fprintf(stderr, "usage: %s <host> <port> <key> <filename>\n", argv[0]);
		exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);
    key = atoi(argv[3]);
    filename = argv[4];

    int request = mycloud_getfile(host, port, key, filename, &data, &datalen);
    
    ofstream outfile(filename);

    outfile.write(data, datalen);

    return request;
}