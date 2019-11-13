#ifndef SPELL_HPP
#define SPELL_HPP
#include "world.hpp"

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
  Type _type;
public:
  Spell(World*, Type);
  void use(Entity* caster, double x, double y);
  Type type();
};

#endif
