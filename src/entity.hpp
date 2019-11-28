#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <ostream>
#include <bitset>
#include <stack>
#include "bounds.hpp"
#include "class_util.hpp"
#include "vec.hpp"
#include "region.hpp"

class Effect;

class Entity {
public:
  enum Components
    {
     NONE,
     LIVES,
     MOVES,
     OCCUPIES,
     COMP_COUNT
    };
  struct Lives {
    bool alive;
    double max_health;
    double health;
    void damage(double dh) {
      health -= dh;
      if(health < 0)
        alive = false;
    }
    void heal(double dh) {
      health += dh;
      if(health > max_health)
        health = max_health;
      else if(health < 0)
        alive = false;
    }
  };
  struct Moves {
    Vec2 velocity;
    double terminal;
    void accelerate(Vec2 dv) {
      velocity = velocity + dv;
      if(Vec2::length(velocity) < terminal)
        velocity = Vec2::normalize(velocity) * terminal;
    }
  };
  struct Occupies {
    Region* region;
  };
  std::bitset<Entity::COMP_COUNT> is_comp;
  Lives lives;
  Moves moves;
  Occupies occupies;
  friend std::ostream& operator<<(std::ostream& os, Entity const& e);
  friend std::ostream& operator<<(std::ostream& os, Lives const& l);
  friend std::ostream& operator<<(std::ostream& os, Moves const& l);
  friend std::ostream& operator<<(std::ostream& os, Occupies const& l);
};

class EntityFactory {
private:
  Entity e;
public:
  EntityFactory* lives(Entity::Lives _lives);
  EntityFactory* moves(Entity::Moves _moves);
  EntityFactory* occupies(Entity::Occupies _occupies);
  Entity create();
};

#endif
