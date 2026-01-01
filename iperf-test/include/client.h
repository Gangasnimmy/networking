#pragma once
#include "common.h"

struct iperf_test;

class client{
    private:
        struct iperf_test* test = NULL;
        std::string output = "";

    public:
    client(const struct config&);
    ~client();
    client& operator=(const client&) = delete;
    client(const client&) = delete;
    
    int run();
    std::string get_output() const;
};