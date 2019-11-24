#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <array>
#include "entity.hpp"
#include "bounds.hpp"
#include "map.hpp"
#include "effect.hpp"


struct Projectile {
  Entity* source;
  std::vector<Effect*>* effects;
  Position position;
  Vec2 velocity;
  Vec2 acceleration;
};


class World {
private:
  Map _map;
  std::vector<MovingEntity*> _moving_entities;
  std::vector<Entity*> _other_entities;
  std::vector<Projectile> _projectiles;
public:
  World();
  std::vector<MovingEntity*>* moving_entities();
  std::vector<Entity*> entities_in_region(Region region);
  void spawn(Entity* entity);
  void spawn(MovingEntity* entity);
  void add_projectile(Projectile projectile);
  void apply_effects(std::vector<Effect*>* effects,
                     Entity* dest,
                     Vec2 target);
  void move_projectiles(double duration);
  std::vector<Projectile>* projectiles();
  Map* map();
};

#endif
