#include "world.hpp"
#include "entity.hpp"
#include "bounds.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include <cmath>


World::World() {}

std::vector<Entity*> World::entities_in_region(Region* region) {
  std::vector<Entity*> out;
  for(Entity* e : entities)
    if(e->is_comp[Entity::OCCUPIES])
      if(region->might_collide(e->occupies.region))
        out.push_back(e);
  return out;
}


void World::spawn(Entity* e) {
  entities.push_back(e);
}

void World::add_projectile(Projectile projectile) {
  projectiles.push_back(projectile);
}

void World::move_projectiles(double duration) {
  for(auto p = projectiles.begin(); p != projectiles.end();) {
    // we will do certain things if a projectile has hit a target
    // if the projectile has a position it wants to travel to,
    // adjust its velocity direction to travel towards that position
    if(p->target != NULL)
      p->velocity = Vec2::normalize(*p->target - p->position) * p->speed;
    // update position based on speed and velocity direction 
    p->position = p->position + p->velocity * duration * p->speed;
    // check if it collided with an entity
    std::vector<unsigned int> temp_hit;
    for(unsigned int i = 0; i < entities.size(); i++) {

      /*
        we want to hit the enemy when they haven't been hit before
       */
      Entity* e = entities[i];
      Vec2 dist = e->occupies.region->position - p->position;
      if(Vec2::dot(dist, dist) < std::pow(e->occupies.region->bounds()->size().x / 2, 2)) {
        auto p_it = std::find(p->prev_hit.begin(), p->prev_hit.end(), i);
        auto t_it = std::find(temp_hit.begin(), temp_hit.end(), i);
        if(e != p->source && p_it == p->prev_hit.end() && t_it == temp_hit.end()) {
          // we hit an entity!
          p->max_hit -= 1;
          for(Effect* ef : *p->on_hit) {
            ef->targets = {e};
            timeline.add(ef->clone(), 0);
          }
          if(p->max_hit == 0)
            break;
        }
        temp_hit.push_back(i);
      }
    }
    p->prev_hit = temp_hit;
    if(p->max_hit == 0) {
      p = projectiles.erase(p);
    } else {
      p++;
    }
  }
}
