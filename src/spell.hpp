#ifndef SPELL_HPP
#define SPELL_HPP

#include "effect.hpp"
#include "region.hpp"
#include "timeline.hpp"
#include "world.hpp"
#include <vector>

class Spell {
  public:
    enum Type { SELF, ENTITY_TARGET, POINT_TARGET, REGION, PROJECTILE };
    Type type;
    std::vector<Effect *> effects;
    Region *region = new Circle(Vec2::zero, 0);
    Vec2 position = Vec2::zero;
    Entity *target;
    Entity *source;
    void use(World *world);
};

#endif
