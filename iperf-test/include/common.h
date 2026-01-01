#pragma once
#include <string>

struct config{
    std::string hostname;
    int port;
    int duration;
    int num_stream;
    bool json_output;
};