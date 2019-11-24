#include "world.hpp"
#include "entity.hpp"
#include "bounds.hpp"
#include <vector>
#include <iostream>
#include <cmath>

World::World() {}

std::vector<Entity*> World::entities_in_region(Region region) {
  std::vector<Entity*> out;
  for(Entity *e : _moving_entities)
    if(region.might_collide(e->region))
      out.push_back(e);
  for(Entity *e : _other_entities)
    if(region.might_collide(e->region))
      out.push_back(e);
  return out;
}

void World::spawn(Entity* e) {
  _other_entities.push_back(e);
}

void World::spawn(MovingEntity* e) {
  _moving_entities.push_back(e);
}

void World::add_projectile(Projectile projectile) {
  _projectiles.push_back(projectile);
}

void World::apply_effects(std::vector<Effect*>* effects,
                          Entity* dest,
                          Vec2 target) {
  for(Effect* e : *effects)
    e->affect(dest, target);
}

void World::move_projectiles(double duration) {
  for(auto p = _projectiles.begin(); p != _projectiles.end();) {
    bool hitp = false;
    p->velocity = p->velocity + p->acceleration * duration;
    p->position = p->position + p->velocity * duration;
    for(Entity* e : entities_in_region({{0, 0}, new RectangularBounds({2, 2})})) {
      if(e != p->source) {
        Vec2 dist = e->region->position - p->position;
        if(Vec2::dot(dist, dist) < std::pow(e->region->bounds()->size().x, 2)) {
          apply_effects(p->effects, e, p->position);
          hitp = true;
          break;
        }
      }
    }
    if(hitp)
      p = _projectiles.erase(p);
    else
      p++;
  }
}

std::vector<MovingEntity*>* World::moving_entities() {
  return &_moving_entities;
}

std::vector<Projectile>* World::projectiles() {
  return &_projectiles;
}

Map* World::map() {
  return &_map;
}
