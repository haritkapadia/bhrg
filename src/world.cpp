#include "world.hpp"
#include "entity.hpp"
#include "bounds.hpp"
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include <cmath>

World::World() {}

std::vector<Entity*> World::entities_in_region(Region region) {
  std::vector<Entity*> out;
  for(Entity* e : entities)
    if(e->is_comp[Entity::OCCUPIES])
      if(region.might_collide(e->occupies.region))
        out.push_back(e);
  return out;
}



// void World::process_effects() {
//   for(auto et = effect_targets.begin(); et != effect_targets.end();) {
//     Effect* effect = et->first;
//     std::cout << *effect << '\n';
//     switch(effect->state) {
//     case Effect::READY:
//       std::for_each(et->second.begin(), et->second.end(),
//                     std::bind(&Effect::start, effect, std::placeholders::_1));
//       et++;
//       break;
//     case Effect::ONGOING:
//       std::for_each(et->second.begin(), et->second.end(),
//                     std::bind(&Effect::pulse, effect, std::placeholders::_1));
//       et++;
//       break;
//     case Effect::COMPLETE:
//       std::for_each(et->second.begin(), et->second.end(),
//                     std::bind(&Effect::stop, effect, std::placeholders::_1));
//       effect_targets.erase(et);
//       break;
//     default:
//       break;
//     }
//   }
// }




void World::spawn(Entity* e) {
  entities.push_back(e);
}

void World::add_projectile(Projectile projectile) {
  projectiles.push_back(projectile);
}

void World::move_projectiles(double duration) {
  for(auto p = projectiles.begin(); p != projectiles.end();) {
    bool hitp = false;
    p->velocity = p->velocity + p->acceleration * duration;
    p->position = p->position + p->velocity * duration;
    for(Entity* e : entities_in_region({{0, 0}, new RectangularBounds({2, 2})})) {
      if(e != p->source) {
        Vec2 dist = e->occupies.region->position - p->position;
        if(Vec2::dot(dist, dist) < std::pow(e->occupies.region->bounds()->size().x, 2)) {
          hitp = true;
          break;
        }
      }
    }
    if(hitp)
      p = projectiles.erase(p);
    else
      p++;
  }
}
