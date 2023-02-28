#include "socket.h"


void ServerSocket::init_server(const char *port){
    this->port=port;
    memset(&this->host_info, 0, sizeof(this->host_info));
    this->host_info.ai_family   = AF_UNSPEC;
    this->host_info.ai_socktype = SOCK_STREAM;
    this->host_info.ai_flags    = AI_PASSIVE;
    this->status = getaddrinfo(this->hostname, this->port, &this->host_info, &this->host_info_list);
    if (this->status != 0) {
        cerr << "[error] cannot get address info for proxy server." << endl;
        return;
    }
    this->socket_fd = socket(this->host_info_list->ai_family, this->host_info_list->ai_socktype, this->host_info_list->ai_protocol);
    if (this->socket_fd == -1) {
        cerr << "[error] cannot create socket." << endl;
        return;
    }
    int yes = 1;
    this->status = setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    this->status = ::bind(this->socket_fd, this->host_info_list->ai_addr, this->host_info_list->ai_addrlen);
    if (this->status == -1) {
        cerr << "[error] cannot bind socket." << endl;
        return;
    }
    this->status = listen(this->socket_fd, 100);
    if (this->status == -1) {
        cerr << "[error] cannot listen socket." << endl;
        return;
    }

    std::cout << "Proxy server is waiting for connection on port " << port << std::endl;
    return;
}

void ClientSocket::init_client(const char *hostname, const char *port){
    
    this->port=port;
    this->hostname=hostname;
    memset(&this->host_info, 0, sizeof(this->host_info));
    this->host_info.ai_family   = AF_UNSPEC;
    this->host_info.ai_socktype = SOCK_STREAM;
    this->status = getaddrinfo(this->hostname, this->port, &this->host_info, &this->host_info_list);
    if (this->status != 0) {
        //cerr << "[error] cannot get address info for web server." << endl;
        return;
    }
    this->socket_fd = socket(this->host_info_list->ai_family, 
		     this->host_info_list->ai_socktype, 
		     this->host_info_list->ai_protocol);
    if (this->socket_fd == -1) {
        cerr << "[error] cannot create socket." << endl;
        return;
    }
    this->status = connect(this->socket_fd, this->host_info_list->ai_addr, this->host_info_list->ai_addrlen);
    if (this->status == -1) {
        cerr << "[error] cannot connect to socket." << endl;
        return;
    }
    else{
        std::cout << "Web server connected to " << hostname << " on port " << port << std::endl;
    
    }
    return;
}

void ServerSocket::accept_conn(MyStruct * data){
    
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd=accept(this->socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (client_connection_fd == -1) {
        cerr << "[error] proxy server cannot accept connection." << endl;
        return;
    }
    data->fd = client_connection_fd;
    data->ip = inet_ntoa(((struct sockaddr_in *)&socket_addr)->sin_addr);
    return;
}

