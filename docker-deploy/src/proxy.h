#include "log.h"
#include "cache.h"
#include "socket.h"
#include "request.h"
#include <string>
#include <pthread.h>
#define MAX_SIZE 65536

using namespace std;
class Cache;
class Request;

#ifndef _MYSTRUCT_H__
#define _MYSTRUCT_H__
class MyStruct {

public:

    Cache *cache;
    string ip;
    int fd;
    int id;
    
};

#endif

void *handle(void * information);
void handle_connect(int id, int server_fd, int client_fd);
void handle_get(int id, int server_fd, int client_fd, Request req, Cache & cache);
void handle_post(int id, int server_fd, int client_fd, Request req);
void recv_complete(int fd, vector<char> &v);
