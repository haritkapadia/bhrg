#include "stopwatch.hpp"
#include <iostream>

void Stopwatch::start(unsigned long long now) {
    total_paused = 0;
    watching = true;
    start_time = now;
    update_now(now);
}

void Stopwatch::pause(unsigned long long now) {
    paused = true;
    pause_time = now;
    update_now(now);
}

void Stopwatch::unpause(unsigned long long now) {
    update_now(now);
    paused = false;
}

void Stopwatch::toggle_pause(unsigned long long now) {
    if (paused)
        unpause(now);
    else
        pause(now);
}

void Stopwatch::update_now(unsigned long long _now) {
    prev = now;
    now = _now;
    if (paused)
        total_paused += now - prev;
    // std::cout << "Update: " << start_time << ','
    //           << now << ','
    //           << total_paused << ','
    //           << now - start_time << ','
    //           << (now - start_time + total_paused) / 1000.0 << '\n';
}

double Stopwatch::elapsed() {
    return (now - start_time + total_paused) / 1000.0;
}

double Stopwatch::diff() { return (now - prev) / 1000.0; }
