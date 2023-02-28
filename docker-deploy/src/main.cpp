#include "proxy.h"

int main(){
    open_file("proxy.log");
    Cache cache= Cache();
    int id = 0;
    
    ServerSocket proxy=ServerSocket();
    proxy.init_server("12345");
    
    //deamon
    if(daemon(1,0) == -1){ //(0,0) msg not shown in terminal,only in log
        std::cout<<"error\n"<<std::endl;
        exit(-1);
    }
    
    
    while(true){
      
            MyStruct * data = new MyStruct();
            
            
            proxy.accept_conn(data);
            
            
            data->id = id++;
            data->cache = &cache;
            
            pthread_t thread;
            pthread_create(&thread, NULL, handle, data);
        
    }
    close(proxy.socket_fd);
    free(proxy.host_info_list);
    return 0;
}