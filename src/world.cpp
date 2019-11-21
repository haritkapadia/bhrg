#include "world.hpp"
#include "entity.hpp"
#include "bounds.hpp"
#include <vector>
#include <iostream>

World::World() {}

std::vector<Entity*> World::entities_in_area(Position position, Bounds* bounds) {
  std::vector<Entity*> out;
  for(Entity *e : _moving_entities)
    if(bounds->contains(position, e->region->position))
      out.push_back(e);
  for(Entity *e : _other_entities)
    if(bounds->contains(position, e->region->position))
      out.push_back(e);
  return out;
}

void World::spawn(Entity* e) {
  _other_entities.push_back(e);
}

void World::spawn(MovingEntity* e) {
  _moving_entities.push_back(e);
}

std::vector<MovingEntity*>* World::moving_entities() {
  return &_moving_entities;
}

Map* World::map() {
  return &_map;
}
