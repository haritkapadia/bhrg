#include "spell.hpp"
#include "world.hpp"
#include "bounds.hpp"
#include "entity.hpp"
#include <iostream>

Spell::Spell(World* world, Spell::Type _type) : world(world), _type(_type) {}

void Spell::use(Entity* caster, double x, double y) {
  for(auto e : world->entities_in_bounds(Bounds(-1, -1, 2, 2))) {
    std::cout << *e << ' ';
    e->x(x);
    e->y(y);
    std::cout << *e << '\n';
  }
}

Spell::Type Spell::type() {
  return _type;
}
