#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <set>
#include <unordered_set>
#include <functional>
#include "entity.hpp"
#include "bounds.hpp"
#include "map.hpp"
#include "effect.hpp"


struct Projectile {
  Entity* source;
  Position position;
  // this is actually just a direction, it is then multiplied by speed
  Vec2 velocity;
  double speed;
  // if target is non-null, velocity will be adjusted
  // to point towards the target based on speed
  Vec2* target = NULL;
  int max_hit = 1;
  std::vector<Effect*>* on_hit;
  // this should probably be a set, but i keep getting segfaults when using sets :(
  std::vector<unsigned int> prev_hit = {};
};

// class HitCooldown : public Event {
// public:
//   unsigned int e_idx;
//   Projectile* p;
//   int i;
//   HitCooldown(unsigned int e_idx, Projectile* p, int i);
//   virtual void act(double progress);
// };

class World {
public:
  World();
  std::vector<Entity*> entities;
  std::vector<Entity*> entities_in_region(Region* region);
  void spawn(Entity* entity);
  void add_projectile(Projectile projectile);
  void move_projectiles(double duration);
  std::vector<Projectile> projectiles;
  Map map;
  Timeline timeline;
};

#endif
