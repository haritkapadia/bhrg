#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <unordered_set>
#include <functional>
#include "entity.hpp"
#include "bounds.hpp"
#include "map.hpp"
#include "effect.hpp"


struct Projectile {
  Entity* source;
  Position position;
  Vec2 velocity;
  double speed;
  // if target is non-null, velocity will be adjusted
  // to point towards the target based on speed
  Vec2* target = NULL;
  int max_hit = 1;
  std::vector<Effect*>* on_hit;
  // std::unordered_set<Entity*> blacklist = {};
  double hit_cooldown = 0.5;
};

class HitCooldown : public Event {
public:
  Entity* e;
  Projectile* p;
  HitCooldown(Entity* e, Projectile* p);
  virtual void act(double progress);
};

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
