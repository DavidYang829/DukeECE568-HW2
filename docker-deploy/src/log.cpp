#include <fstream>
#include <stdexcept>
#include <system_error>

#include "log.h"

ofstream file;
mutex thread_mutex;


void open_file(string filepath){

  try {
      file.open(filepath);
  } 
  catch (const exception &e) {
      throw std::runtime_error("Failed to open file " + filepath + ": " + e.what());
  }
  
}


void make_log(int id, string info){
    stringstream stream;
    stream << id << ":" << info << endl;
    string msg = stream.str();
    stream.str("");
    lock_guard<mutex> lck(thread_mutex);
    
    file << msg;
    file.flush();
}



string get_curr_time() {
  time_t curr_time = time(nullptr);
  array<char, 80> buffer{};
  strftime(buffer.data(), buffer.size(), "%a %b %d %H:%M:%S %Y", localtime(&curr_time));
  return string(buffer.data());
}


