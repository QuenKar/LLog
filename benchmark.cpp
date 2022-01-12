#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <cstdio>

#include "LLog.hpp"

uint64_t timestamp_now()
{
    return std::chrono::high_resolution_clock::now().time_since_epoch() / std::chrono::microseconds(1);
}

void llog_benchmark()
{
    int const iterations = 100000;
    char const *const benchmark = "benchmark";
    uint64_t begin = timestamp_now();

    for (int i = 0; i < iterations; ++i)
    {
        LOG_INFO << "Logging" << benchmark << i << 0 << 'K' << -42.42;
    }

    uint64_t end = timestamp_now();
    long int avg_latency = (end - begin) * 1000 / iterations;
    printf("\tAverage LLog Latency = %ld nanoseconds\n", avg_latency);
}

template <typename Function>
void run_benchmark(Function &&f, int thread_count)
{
    printf("Thread count: %d\n", thread_count);
    std::vector<std::thread> threads;
    for (int i = 0; i < thread_count; ++i)
        threads.emplace_back(f);
    for (int i = 0; i < thread_count; ++i)
        threads[i].join();
}

int main()
{
    llog::initialize(llog::NonGuaranteedLogger(10), "/home/quenkar/code/Log/LLog/", "llog", 1);
    for (auto threads : {1, 2, 3, 4, 5})
        run_benchmark(llog_benchmark, threads);

    return 0;
}