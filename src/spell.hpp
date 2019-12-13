#ifndef SPELL_HPP
#define SPELL_HPP

#include "effect.hpp"
#include "timeline.hpp"
#include "region.hpp"
#include "world.hpp"
#include <vector>

class Spell {
public:
  enum Type {SELF, ENTITY_TARGET, POINT_TARGET, REGION, PROJECTILE};
  Type type;
  std::vector<Effect*> effects;
  Region* region = new Region(Vec2::zero, new CircularBounds(0));
  Entity* target;
  Entity* source;
  void use(World* world);
};

#endif
