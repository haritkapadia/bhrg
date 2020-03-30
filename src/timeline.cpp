#include "timeline.hpp"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <queue>
#include <vector>

Event::Event(double start, double duration) : start(start), duration(duration) {}

bool operator<(const Event &l, const Event &r) { return l.start < r.start; }

std::ostream &operator<<(std::ostream &os, Event const &e) {
    return os << "(" << e.start << "s for " << e.duration << "s)";
}

void Timeline::process() {
    double elapsed = Stopwatch::elapsed();
    // For each event currently running, act
    // If the the event is done, make sure to pass act(1.0), then remove
    for (auto et = present.begin(); et != present.end();) {
        double action_progress = (elapsed - (*et)->start) / (*et)->duration;
        bool destroy = false;
        if (action_progress >= 1) {
            action_progress = 1;
            destroy = true;
        }
        (*et)->act(action_progress);
        // swap with last element and pop last element
        // in present, order does not matter and is not preserved
        if (destroy) {
            std::swap(*et, *(present.end() - 1));
            present.pop_back();
        } else {
            et++;
        }
    }
    // Add events that have reached the present to the running events
    Event *e;
    while (!future.empty() && (e = future.top())->start < elapsed) {
        future.pop();
        e->act(0);
        // instant events have 0 duration
        // they should not occur more than once
        if (e->duration != 0)
            present.push_back(e);
    }
}

void Timeline::add(Event *e) { future.push(e); }

void Timeline::add(Event *e, double dt) {
    e->start = Stopwatch::elapsed() + dt;
    future.push(e);
}
