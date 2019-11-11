#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <array>
#include "entity.hpp"
#include "bounds.hpp"

class World {
private:
  std::vector<Player*> _players;
  std::vector<MovingEntity*> _moving_entities;
  std::vector<Entity*> _other_entities;
public:
  World();
  std::vector<MovingEntity*>* moving_entities();
  std::vector<Entity*> entities_in_bounds(Bounds bounds);
  void spawn(Entity* entity);
  void spawn(MovingEntity* entity);
  void spawn(Player* entity);
  Player* player(int i);
};

#endif
