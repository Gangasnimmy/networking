#pragma once
#include "common.h"
#include <atomic>

struct iperf_test;

class server{
    private:
        struct iperf_test* test = NULL;
        std::atomic<bool> failed = false;


    public:
        server(int);
        ~server();
        server& operator=(const server&) = delete;
        server(const server&) = delete;
        bool has_failed() const;
        void run();

};