// timer.cpp
#include "timer.h"

Timer::Timer() {
    start_time = std::chrono::high_resolution_clock::now();
}

void Timer::start() {
    if (!running) {
        running = true;
        start_time = std::chrono::high_resolution_clock::now();
    }
}

void Timer::pause() {
    if (running) {
        auto now = std::chrono::high_resolution_clock::now();
        total_elapsed += std::chrono::duration<double>(now - start_time).count();
        running = false;
    }
}

double Timer::elapsed() const {
    if (running) {
        auto now = std::chrono::high_resolution_clock::now();
        return total_elapsed + std::chrono::duration<double>(now - start_time).count();
    }
    return total_elapsed;
}
