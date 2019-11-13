#include "spell.hpp"
#include "world.hpp"
#include "bounds.hpp"
#include "entity.hpp"
#include <iostream>

Spell::Spell(World* world, Spell::Type _type) : world(world), _type(_type) {}

void Spell::use(Entity* caster, double x, double y) {
  auto ents = world->entities_in_area({-1, -1}, new RectangularBounds(2, 2));
  for(auto e : ents)
    e->position({x, y});
}

Spell::Type Spell::type() {
  return _type;
}
