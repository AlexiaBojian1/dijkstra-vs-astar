// timer.h
#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
public:
    Timer();

    void start();
    void pause();
    double elapsed() const;

private:
    bool running = false;
    double total_elapsed = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

#endif // TIMER_H
