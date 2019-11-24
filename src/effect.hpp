#ifndef EFFECT_HPP
#define EFFECT_HPP
#include "entity.hpp"

class Effect {
public:
  virtual void affect(Entity* dest, Vec2 target) = 0;
};

class Teleport : public Effect {
public:
  virtual void affect(Entity* dest, Vec2 target);
};

class Damage : public Effect {
public:
  double damage;
  Damage(double damage);
  virtual void affect(Entity* dest, Vec2 target);
};

#endif
