#ifndef TIMELINE_HPP
#define TIMELINE_HPP

#include "stopwatch.hpp"
#include <queue>
#include <vector>
#include <ostream>

class Event {
public:
  double start;
  double duration;
  Event(double start, double duration);
  virtual void act(double progress) = 0;
  friend bool operator<(Event const& l, Event const& r);
  friend std::ostream& operator<<(std::ostream& os, Event const& e);
  std::string name = "blank";
};

class Timeline : public Stopwatch {
public:
  std::priority_queue<Event*> future;
  std::vector<Event*> present;
  void process();
  void add(Event* e);
  void add(Event* e, double dt);
};

#endif
