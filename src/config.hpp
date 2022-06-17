#pragma once

struct SolverConfig{
    static SolverConfig global;
    double maxMemoryInGb = 31.0;
    int targetThreads = 8;
};