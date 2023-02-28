#include "cache.h"

mutex mutex1;
using namespace std;

void Cache::insert_cache(string k,string v){ 
  lock_guard<mutex> lck(mutex1);
  
  mycache[k] = v;
  
  cout<<"[cache] after insert new value to map"<<endl;
  for(map<string,string>::const_iterator it = mycache.begin(); it != mycache.end(); it++){
    cout << "-----"<<it->first << "---" << it->second <<endl;
}

}

string Cache::find_cache(string k){
  lock_guard<mutex> lck(mutex1);
  if (mycache.count(k)!=0) {
    return mycache[k];
  }
  return "";
}


bool Cache::try_save_cache(int id, string uri,Response response){
  
  bool can_cache=!response.check_str("no-store")&&!response.check_str("private")&&response.get_http_code()=="200";
  if(!can_cache){
   
    if(response.check_str("no-store")){
      make_log(id,"not cacheable because: Cache-control is no-store");
    }
    else if(response.check_str("private")){
     
      make_log(id,"not cacheable because: Cache-control is private");
    }
    
    
    return false;
    
  }
  
  else{
    insert_cache(uri,response.first_line);
    
    if(response.find_str("Expires")!=""){
     make_log(id,"cached, expires at "+response.find_str("Expires"));
    }
    else{
      make_log(id,"cached, but requires re-validation");
    }
    return true;
  }
}



string Cache::try_validate(int id, int server_fd, Request request,Response response){
  
  
  //receive from web server
  vector<char> recv_vec;
  recv_complete(server_fd,recv_vec);
  
  string response_str;
  response_str.insert(response_str.begin(), recv_vec.begin(), recv_vec.end());
  Response newResponse(response_str);
  if(newResponse.get_http_code()!="304"){
    try_save_cache(id, request.uri,newResponse);
    return newResponse.complete_message;
  }
  
    return response.complete_message;
  
}
string Cache::revalidate(int id, int server_fd,  Response res, Request req){
  string etag=res.find_str("ETag");
  string last_mod=res.find_str("Last-Modified");
  if(last_mod!=""){
    //send new req to web server
    string req_new=req.get_header()+"\r\n"+"If-Modified-Since: "+last_mod+"\r\n\r\n";
    send(server_fd,req_new.data(),req_new.size()+1,0);
    return try_validate(id, server_fd, req,res);
  }
  else if(etag!=""){
    string req_new=req.get_header()+"\r\n"+"If-None-Match: "+etag+"\r\n\r\n";
    send(server_fd,req_new.data(),req_new.size()+1,0);
    return try_validate(id, server_fd, req,res);
  }
  
  else{
    //resend req to the server
    cout<<"[info] resend request to "<<server_fd<<endl;
    string origin=req.complete_message;
    
    send(server_fd,origin.data(),origin.size()+1,0);
    vector<char> recv_vec;
    recv_complete(server_fd,recv_vec);
    
    string response_str;
    response_str.insert(response_str.begin(), recv_vec.begin(), recv_vec.end());
    Response newResponse(response_str);
    try_save_cache(id, req.uri,newResponse);
    return newResponse.complete_message;
  }
}


