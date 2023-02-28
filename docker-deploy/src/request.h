#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__
#include <iostream>
#include <string>
#include "socket.h"

using namespace std;

class Request{

public:

  string complete_message;
  string line;
  string method;
  string hostname;
  string port;
  string uri;
  
  
public:

  
  Request(string _request):complete_message(_request){
  
    init();
  }
   
  void init();
  void find_addr();
  string get_header();
  bool check_valid();
  int get_content_len();
};

#endif