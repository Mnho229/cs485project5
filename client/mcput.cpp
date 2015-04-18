#include "csapp.h"
#include "mycloud.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>

using namespace std;

int main(int argc, char **argv) 
{
    int clientfd, port, key;
    char *host, filename;
    rio_t rio;

    if (argc != 4) {
		fprintf(stderr, "usage: %s <host> <port> <key> <filename>\n", argv[0]);
		exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);
    key = atoi(argv[3]);
    filename = argv[4];

    fstream queryFile;

    queryFile.open(filename);

    string temp;
    string data;

    while(getline(queryFile, temp)) {
        data = data + temp + "\n";
    }

    mycloud_putfile(host, port, key, filename, data.c_str(), data.length() );
    exit(0);
}