#include <thread>
#include "proxy.h"
#include "socket.h"

void recv_complete(int fd, vector < char > & msg_vec) {

  msg_vec.resize(MAX_SIZE);

  size_t len = recv(fd, & msg_vec.data()[0], msg_vec.size(), 0);
  if (len <= 0) {
    cerr << "[error] cannot recv in handle get" << endl;
    return;
  }

  string res_str;
  res_str.insert(res_str.begin(), msg_vec.begin(), msg_vec.end());
  Response res_obj = Response(res_str);

  size_t i = len;

  if (!res_obj.is_chunk()) {
    int content_len = res_obj.get_con_len();
    cout << "[info] get response not chunked, with con_len: " << len << endl;
    while (i < content_len) {
      msg_vec.resize(i + MAX_SIZE);
      len = recv(fd, & msg_vec.data()[i], msg_vec.size(), 0);

      if (len <= 0) {
        //cerr << "[error] cannot recv chunk in handle get" << endl;
        return;
      }
      i = i+ len;
    }
  } 
  
  else {
    cout << "[info] get response  is chunked, no con_len" << endl;
    string recv_str;
    recv_str.insert(recv_str.begin(), msg_vec.begin(), msg_vec.end());
    while (recv_str.find("0\r\n\r\n") == string::npos) {
      msg_vec.resize(i + MAX_SIZE);
      len = recv(fd, & msg_vec.data()[i], MAX_SIZE, 0);

      if (len <= 0) {break; }
      recv_str = "";
      recv_str.insert(recv_str.begin(), msg_vec.begin() + i, msg_vec.begin() +len+i);
      i += len;

    }
    i = i+ len;
  }

}

void handle_get(int id, int server_fd, int client_fd, Request req, Cache & cache) {

  string response = cache.find_cache(req.uri);

  if (response != "") {
  
    cout << "[info] get response in cache"<< endl;
    Response reponse_obj(response);
    if (reponse_obj.need_revalid(id)) {
      response = cache.revalidate(id, server_fd, reponse_obj,req);
      
    }
    else{
    
    make_log(id,"in cache, valid");
    }
  } 
  
  
  else {
    cout << "[info] get response not in cache"<< endl;
    make_log(id, "not in cache");
    send(server_fd, req.complete_message.data(), req.complete_message.size() + 1, 0);
    make_log(id,"Requesting "+req.line+" from "+req.hostname);
    vector < char > recv_vec;

    recv_complete(server_fd, recv_vec);

    string response_str;
    response_str.insert(response_str.begin(), recv_vec.begin(), recv_vec.end());
    Response rsp(response_str);
    make_log(id,"Received "+rsp.first_line+" from "+req.hostname);
    response = string(recv_vec.begin(), recv_vec.end());
    cache.try_save_cache(id, req.uri, rsp);
  }
  
  /*
  cout << "[yes] below is response [yes]" << endl;
  cout << response.substr(0, 400) << endl;
  cout << "[yes] end of response [yes]" << endl;
  */

  Response rsp(response);
  send(client_fd, response.data(), response.size() + 1, 0);
  cout << "[info] proxy handle get success" << endl;
}

void handle_connect(int id, int server_fd, int client_fd) {
  
  if (send(client_fd, "HTTP/1.1 200 OK\r\n\r\n", 19, 0) < 0) {
    cerr << "[error] cannot send 200 to " << client_fd << endl;
    return;
  }
  make_log(id,"Responding HTTP/1.1 200 OK\r\n\r\n");

  fd_set readfds;
  int nfds = server_fd > client_fd ? server_fd : client_fd;
  while (true) {
    FD_ZERO( & readfds);
    FD_SET(server_fd, & readfds);
    FD_SET(client_fd, & readfds);
    select(nfds + 1, & readfds, NULL, NULL, NULL);
    int fd_list[2] = {
      server_fd,
      client_fd
    };

    for (int i = 0; i < 2; i++) {
      char message[MAX_SIZE] = {
        0
      };
      if (FD_ISSET(fd_list[i], & readfds)) {
        int recv_len = recv(fd_list[i], message, sizeof(message), 0);
        if (recv_len <= 0) {
          return;
        } else {
          if (send(fd_list[1 - i], message, recv_len, 0) <= 0) {
            return;
          }
        }
      }
    }
  }
  
  
}

void handle_post(int id, int server_fd, int client_fd, Request req) {

  string response_str = req.complete_message;

  if (req.get_content_len() != -1) {
    char response_char[MAX_SIZE] = {0};
    send(server_fd, response_str.data(), response_str.size() + 1, 0);

    int rsp_len = recv(server_fd, response_char, sizeof(response_char), MSG_WAITALL);
    if (rsp_len != 0) {
    
    
      make_log(id,response_char);
      Response res(response_char);
      send(client_fd, response_char, rsp_len, 0);
      make_log(id,"Received "+res.first_line+" from "+req.hostname);
      cout << "[info] proxy handle post success" << endl;
    } else {
      cerr << "[error] cannot post with socket " << client_fd << endl;
      return;
    }
  }
}

void * handle(void * data_) {

  MyStruct * data = (MyStruct * ) data_;

  char request_char[MAX_SIZE] = {
    0
  };
  int len = recv(data -> fd, request_char, sizeof(request_char), 0);
  if (len == -1) {
    cerr << "[error] invalid request." << endl;
    return NULL;
  }
  Request request_obj(string(request_char, len));
  make_log(data -> id, request_obj.line+" from "+data -> ip+ "@"+get_curr_time());

  ClientSocket client = ClientSocket();
  client.init_client(request_obj.hostname.c_str(), request_obj.port.c_str());

  if (request_obj.method == "CONNECT") {

    handle_connect(data -> id, client.socket_fd, data -> fd);
    make_log(data -> id, " Tunnel closed");
    //cout<<"[yes] tunnel close!"<<endl;

  } else if (request_obj.method == "GET") {

    handle_get(data -> id, client.socket_fd, data -> fd, request_obj, * data -> cache);

  } else if (request_obj.method == "POST") {

    handle_post(data -> id, client.socket_fd, data -> fd,request_obj);

  }
  close(client.socket_fd);
  free(client.host_info_list);
  return NULL;
}