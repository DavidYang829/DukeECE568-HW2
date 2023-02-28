#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "response.h"
#include "proxy.h"


using namespace std;

class Socket{
public:
    int status;
    int socket_fd;
    const char *hostname;
    const char *port;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    
};

class ServerSocket : public Socket{
public:

    void init_server(const char *port);
    void accept_conn(MyStruct * data);
};

class ClientSocket : public Socket{
public:
    void init_client(const char *hostname, const char *port);
};


#endif

