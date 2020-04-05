#include "event.hpp"

Event::Event(long long start, unsigned long long duration) : start(start), duration(duration) {
    name = "Event";
}

bool operator<(const Event &l, const Event &r) { return l.start < r.start; }

std::ostream &operator<<(std::ostream &os, Event const &e) {
    return os << "(" << e.start << "s for " << e.duration << "s)";
}
