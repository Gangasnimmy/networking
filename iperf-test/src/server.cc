#include "server.h"
#include <iostream>
#include <stdint.h>
#include <iperf_api.h>

server::server(int p){
    if((test = iperf_new_test()) != nullptr){
        iperf_defaults(test);
        iperf_set_test_role(test,'s');
        iperf_set_test_server_port(test,p);
    }
}

server::~server(){
    if(test)
        iperf_free_test(test);
}
 
void server::run(){
    if(!test)
        return;

    if(iperf_run_server(test) < 0){
            std::cerr<<"Failed to start server :"<<iperf_strerror(i_errno)<<std::endl;
            failed.store(true);  
    }
}

bool server::has_failed() const{
    return failed.load();
}

