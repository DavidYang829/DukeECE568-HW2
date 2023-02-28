#include "request.h"
void Request::init(){
size_t f=complete_message.find("HTTP/1.1");
    line=complete_message.substr(0,f+8);
    
    size_t pos1=line.find(' ');
    method=line.substr(0,pos1);
    
    size_t uri_start=line.find(' ')+1;
    size_t uri_end=line.find(' ',uri_start);
    uri=line.substr(uri_start,uri_end-uri_start);
    
    find_addr();
}

void Request::find_addr(){
    try {
    size_t host_start;
    size_t host_end;
    std::string host_line;
    host_start = complete_message.find("Host:") + 6; //"Host: " has 6 char
    std::string after_host = complete_message.substr(host_start);
    host_end = after_host.find("\r\n");
    host_line = after_host.substr(0, host_end);
    size_t port_start = host_line.find(":");
    if (port_start != std::string::npos) {
      hostname = after_host.substr(0, port_start);
      port = host_line.substr(port_start + 1);
    }
    else {
      hostname = host_line;
      port = "80";
    }
    
  }
  catch (std::exception & e) {
    hostname = "";
    port = "";
    return;
  }
}

bool Request::check_valid() {
  return (method == "CONNECT" || method == "GET" || method == "POST");
}

string Request::get_header(){
  size_t header_end=complete_message.find("\r\n\r\n");
  return complete_message.substr(0,header_end);
}

int Request::get_content_len() {
    
    if (complete_message.find("Content-Length: ")!= std::string::npos) {
    
        size_t contentNum_start = complete_message.find("Content-Length: ")+16;// 16 char
        size_t contentNum_end = complete_message.find("\r\n",contentNum_start);
        size_t contentNum_len = contentNum_end-contentNum_start;
        string contentNum = complete_message.substr(contentNum_start,contentNum_len);
        int content_Num = stoi(contentNum);//get number of content length
        size_t header_end = complete_message.find("\r\n\r\n") + 8; //8 char
        int content_len_count = complete_message.size() - int(header_end) + 4;//.size() omit /r/n
        int check = content_Num - content_len_count;
        return check;//only when count = get, check = 0
        
    }
    return -1;
    
    
    
}
