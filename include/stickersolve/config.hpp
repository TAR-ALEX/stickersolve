#pragma once

#include <estd/ostream_proxy.hpp>
#include <estd/ptr.hpp>
#include <estd/thread_pool.hpp>
#include <iostream>
#include <string>

class SolverConfig {
public:
    estd::ostream_proxy log{&std::cerr};
    double maxMemoryInGb = 31.0;
    std::string pruiningTablesPath = "./";
    estd::clone_ptr<estd::thread_pool> threadPool{8};
    bool useMmapForPruning = false;
};