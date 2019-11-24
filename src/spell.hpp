#ifndef SPELL_HPP
#define SPELL_HPP

#include "world.hpp"
#include "effect.hpp"
#include <vector>

class Spell {
public:
  enum Type
    {
     SELF,
     PROJECTILE,
     TARGET
    };
private:
  World* world;
public:
  Type type;
  Projectile projectile;
  std::vector<Effect*> effects;
  double cooldown;
  Spell(World* world, Type type, std::vector<Effect*> effects, double cooldown);
  void use(Entity* caster, Vec2 target);
};

#endif
