#include "client.h"
#include <iostream>
#include <stdint.h>
#include <iperf_api.h>

client::client(const struct config& c){
    if((test = iperf_new_test()) != nullptr){
        iperf_defaults(test);
        iperf_set_test_role(test,'c');
        iperf_set_test_server_hostname(test,c.hostname.c_str());
        iperf_set_test_server_port(test,c.port);
        iperf_set_test_json_output(test,c.json_output);
        iperf_set_test_duration(test,c.duration);
        iperf_set_test_num_streams(test,c.num_stream);
    }
}
    
client::~client(){
    if(test)
        iperf_free_test(test);
}

int client::run(){

    if(!test)
        return -1;

    if(iperf_run_client(test) < 0){
        std::cerr<<"Failed to start client : "<<iperf_strerror(i_errno);
        return -1;
    }
    char* json =iperf_get_test_json_output_string(test);
    if(json){
        output = json;
    }
    return 0;
}

std::string client::get_output() const{
    return output;
}
