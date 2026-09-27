#include "meng.h"
#include <chrono>
#include <iostream>
#include <iomanip>

static const int NUM_SWITCHES = 10000000;

static void bench_worker(meng* m, void* arg, size_t argsize) {
    (void)arg;
    (void)argsize;
    for (int i = 0; i < NUM_SWITCHES; ++i) {
        meng_yield(m);
    }
}

int main() {
    std::cout << "=== libmeng Context Switch Benchmark ===" << std::endl;
    std::cout << "Total switches: " << NUM_SWITCHES * 2 << " (ping-pong)" << std::endl;

    meng* m = meng_create(bench_worker, 32 * 1024, nullptr, 0);

    auto start = std::chrono::high_resolution_clock::now();

    while (!meng_end(m)) {
        meng_run(m);
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> elapsed_ns = finish - start;
    double total_sec = elapsed_ns.count() / 1e9;

    double switches = static_cast<double>(NUM_SWITCHES) * 2.0;
    double ns_per_switch = elapsed_ns.count() / switches;
    double switches_per_sec = switches / total_sec;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Elapsed time: " << total_sec << " s" << std::endl;
    std::cout << "Latency:      " << ns_per_switch << " ns / context switch" << std::endl;
    std::cout << "Throughput:   " << switches_per_sec / 1e6 << " M switches / sec" << std::endl;

    meng_delete(m);
    return 0;
}
