#include "world.hpp"
#include "entity.hpp"
#include "bounds.hpp"
#include <vector>
#include <iostream>

#define ThroughVector(ve, out)                          \
  for(auto e = (ve).begin(); e != (ve).end(); e++)      \
    if(b.contains((**e).x(), (**e).y()))                \
      (out).push_back(*e);

World::World() {}

std::vector<Entity*> World::entities_in_bounds(Bounds b) {
  std::vector<Entity*> out;
  ThroughVector(_players, out);
  ThroughVector(_moving_entities, out);
  ThroughVector(_other_entities, out);
  return out;
}

#undef ThroughVector

void World::spawn(Entity* e) {
  _other_entities.push_back(e);
}

void World::spawn(MovingEntity* e) {
  std::cout << "Added MovingEntity " << *e << '\n';
  _moving_entities.push_back(e);
}

void World::spawn(Player* e) {
  _players.push_back(e);
  _moving_entities.push_back(e);
}

std::vector<MovingEntity*>* World::moving_entities() {
  return &_moving_entities;
}

Player* World::player(int i) {
  return *(_players.begin() + i);
}
