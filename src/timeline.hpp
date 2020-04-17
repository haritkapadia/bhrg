#ifndef TIMELINE_HPP
#define TIMELINE_HPP

#include "entity.hpp"
#include "event.hpp"
#include "stopwatch.hpp"
#include <array>
#include <istream>
#include <ostream>
#include <queue>
#include <vector>

class World;

class SpawnEnemy1 : public Event {
  public:
    EntityFactory *_enemy;
    Entity *enemy;
    World *world;
    SpawnEnemy1(unsigned long long start, unsigned long long duration, World *world,
                EntityFactory *_enemy);
    virtual void act(double progress);
};

class Timeline : public Stopwatch {
  public:
    static const std::array<std::string, 2> KNOWN_EVENT;
    World *world;
    // this would be a good place to use unique_ptr
    std::vector<Event *> future;
    std::vector<Event *> present;
    void process();
    void add(Event *e);
    void add(Event *e, unsigned long long dt);
    void read(std::istream *fin);
    void write(std::ostream *fout);
};

#endif
