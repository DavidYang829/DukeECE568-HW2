#ifndef __LOG_H__
#define __LOG_H__

#include <mutex>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

void open_file(string path);
void make_log(int id, string msg);
string get_curr_time();


#endif
