#include "response.h"


string Response::get_http_code(){
  int begin=complete_message.find(' ') + 1;
  int end=complete_message.find(' ',begin);
  return complete_message.substr(begin,end-begin);
}


string Response::find_str(string str){
  size_t begin=complete_message.find(str);
  if(begin!=string::npos){
    size_t end=complete_message.find("\r\n",begin+str.size());
    size_t result_begin = begin+str.size()+2;
    return complete_message.substr(result_begin,end - result_begin);
  }
  return "";
}

bool Response::check_str(string str){
  string tag=find_str("Cache-Control");
  if(tag != ""){
    size_t pos=tag.find(str);
    if(pos == string::npos){
      return false;
    }
  }
  return true;
}


bool Response::is_new(int id){ 
  
    bool is_new = false; 
    
    string expire=find_str("Expires");
    string last_modify=find_str("Last-Modified");
	  string max_age=get_max_age();
     
    //double curr_age = get_curr_age(); 
    //get curr age
    time_t date_time=get_utc_time(find_str("Date"));
    time_t curr_time = time(NULL) - 8 * 3600; //8 hours
    double curr_age = difftime(curr_time,date_time);
    
    time_t expire_time = get_utc_time(expire);
    double new_time=difftime(date_time,get_utc_time(last_modify))/10.0; 
	
	
  if(expire!=""){
      
      is_new=curr_time<expire_time;
      if (expire_time < curr_time){
        make_log(id,"in cache, but expired at"+expire);
      }
      return is_new;
  }
  if(last_modify!=""){
      
      is_new=curr_age<new_time;
      if(new_time<date_time){
        make_log(id,"in cache, requires validation");
      return is_new;
  }
   if(max_age!=""){
      
      long max_age_long = stol(max_age); 
      is_new = curr_age<max_age_long;
      if (max_age_long < curr_age){
        make_log(id,"in cache, requires validation");
      }
      return is_new;
  }
    
    
    
    }
    
      return is_new;
    
}




string Response::get_max_age(){
  string tag=find_str("Cache-Control");
  if(!tag.empty()){
    
  //Cache-Control: no-cache, max-age=3600, must-revalidate
  //Cache-Control: no-cache max-age=3600 must-revalidate
  //Cache-Control: no-cache,max-age=3600,must-revalidate
    size_t start=tag.find("max-age")+8;// 8 char
    if(start!=string::npos){
      size_t pos_age_end=tag.find(" ",start+1); 
      //Cache-Control: no-cache, max-age=1200
      if(pos_age_end==string::npos){
        pos_age_end=tag.find("\r\n",start+1);
      }
      size_t max_age_len = pos_age_end-start;
      return tag.substr(start,max_age_len);
    }
  }
  
    
    return "";
}



time_t Response::get_utc_time(string time_str){
  tm utc_time={};
  size_t GMT_pos=time_str.find("GMT");
  if(GMT_pos!=string::npos){
    time_str.erase(GMT_pos - 1,4);
  }
  strptime(time_str.c_str(),"%a, %d %b %Y %H:%M:%S", &utc_time);
  utc_time.tm_isdst = 0; // Disable DST adjustment
  return mktime(&utc_time);// Use timegm to avoid DST adjustment
}


bool Response::need_revalid(int id){
  if(check_str("no-cache")){
    make_log(id,"in cache, requires validation");
    return true;
  }
  
  return !is_new(id);
}


int Response::get_con_len(){
  string len=find_str("Content-Length");
  if(!len.empty()){
  return stoi(len);
  }
  return -1;
}

bool Response::is_chunk() {
  string encode = find_str("Transfer-Encoding");
  return (encode.find("chunked") != string::npos);
}


