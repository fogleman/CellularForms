#include "util.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <random>

void Panic(const std::string &message) {
    std::cerr << message << std::endl;
    std::exit(1);
}

std::function<void()> Timed(const std::string &message) {
    const auto startTime = std::chrono::steady_clock::now();
    return [message, startTime]() {
        return;
        const std::chrono::duration<double> elapsed =
            std::chrono::steady_clock::now() - startTime;
        const double millis = elapsed.count() * 1000;
        std::cerr << message << ": " << millis << "ms" << std::endl;
    };
}

double Random(const double lo, const double hi) {
    // static thread_local std::mt19937 gen(0);
    static thread_local std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist(lo, hi);
    return dist(gen);
}

int RandomIntN(const int n) {
    // static thread_local std::mt19937 gen(0);
    static thread_local std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, n - 1);
    return dist(gen);
}
