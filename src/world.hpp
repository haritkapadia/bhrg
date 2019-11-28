#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <map>
#include "entity.hpp"
#include "bounds.hpp"
#include "map.hpp"


struct Projectile {
  Entity* source;
  Position position;
  Vec2 velocity;
  Vec2 acceleration;
};


class World {
public:
  World();
  std::vector<Entity*> entities;
  std::vector<Entity*> entities_in_region(Region region);
  void spawn(Entity* entity);
  void add_projectile(Projectile projectile);
  void move_projectiles(double duration);
  std::vector<Projectile> projectiles;
  Map map;
};

#endif
