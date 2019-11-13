#include "world.hpp"
#include "entity.hpp"
#include "bounds.hpp"
#include <vector>
#include <iostream>

#define ThroughVector(p, b, ve, out)                                    \
  for(auto e = (ve).begin(); e != (ve).end(); e++)                      \
    if((b)->contains(p, {(**e).position().x, (**e).position().y}))      \
      (out).push_back(*e);

World::World() {}

std::vector<Entity*> World::entities_in_area(Position position, Bounds* bounds) {
  std::vector<Entity*> out;
  ThroughVector(position, bounds, _moving_entities, out);
  ThroughVector(position, bounds, _other_entities, out);
  return out;
}

#undef ThroughVector

void World::spawn(Entity* e) {
  _other_entities.push_back(e);
}

void World::spawn(MovingEntity* e) {
  _moving_entities.push_back(e);
}

std::vector<MovingEntity*>* World::moving_entities() {
  return &_moving_entities;
}
