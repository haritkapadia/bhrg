#ifndef EVENT_HPP
#define EVENT_HPP

#include <iostream>

class Event {
  public:
    long long start;
    unsigned long long duration;
    Event(long long start, unsigned long long duration);
    virtual void act(double progress) = 0;
    friend bool operator<(Event const &l, Event const &r);
    friend std::ostream &operator<<(std::ostream &os, Event const &e);
    std::string name;
};

#endif
