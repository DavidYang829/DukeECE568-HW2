#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include "request.h"
#include <ctime>

using namespace std;
class Response{
public:

  string complete_message;
  string first_line;
  
  Response(string _response):complete_message(_response),first_line(""){
  
  first_line=complete_message.substr(0,complete_message.find("\n")-1);
  }
  
  
  bool is_chunk();
  bool is_new(int id);
  bool check_str(string str);
  string find_str(string str);
  
  int get_con_len();
  string get_max_age();
  string get_http_code();
  time_t get_utc_time(string strTime);
  
  bool need_revalid(int thread_id);
  

};



#endif


