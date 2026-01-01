#include <iostream>
#include <fstream>
#include <string>
#include "server.h"
#include "client.h"
#include <unistd.h>
#include <chrono>
#include <thread>
#include <errno.h>

int main(){
    
    server myServer(5050);
    std::thread serverThread(&server::run, &myServer);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if(myServer.has_failed()){
        std::cerr<<"Server failed, aborting..."<<std::endl;
        return -1;
    }

    struct config myconfig;
    myconfig.hostname = "127.0.0.1";
    myconfig.port = 5050;
    myconfig.duration = 2;
    myconfig.num_stream = 2;
    myconfig.json_output = 1;

    client myClient(myconfig);
    if(myClient.run() < 0){
        std::cerr<<"client failed to start"<<std::endl;
        return -1;
    }

    std::string output = myClient.get_output();

    std::ofstream outfile("../output.txt");

    if(!outfile.is_open()){
        std::cerr<<"Failed to open a file"<<std::endl;
        return -1;
    }
 
    outfile << output << std::endl;  

    outfile.close();
    serverThread.join();

    return 0;
}