#pragma once

#include <estd/ostream_proxy.hpp>
#include <estd/ptr.hpp>
#include <estd/thread_pool.hpp>
#include <iostream>
#include <string>
#include <thread>

class SolverConfig {
public:
    estd::ostream_proxy log{&std::cerr};
    double maxMemoryInGb = 31.0;
    std::string pruiningTablesPath = "./";
    estd::clone_ptr<estd::thread_pool> threadPool{8};
    bool useMmapForPruning = false;
    SolverConfig() {
        int hwt = (int)std::thread::hardware_concurrency();
        if (hwt != 0) threadPool = estd::thread_pool{hwt+1};
    }
};