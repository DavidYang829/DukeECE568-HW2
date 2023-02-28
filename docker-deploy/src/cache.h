#ifndef __CACHE_H__
#define __CACHE_H__

#include <mutex>
#include <map>

#include "log.h"
#include "proxy.h"
#include "request.h"
#include "response.h"

class Response;
class Request;
class Cache{
  private:
    
    map<string,string> mycache;
    
  public:
  
    string find_cache(string k);
    void insert_cache(string k,string v);
    bool try_save_cache(int id, string uri,Response rsp);
    string try_validate(int id, int server_fd, Request req,Response res);
    string revalidate(int id, int fd,  Response res, Request req);
    
};

#endif
