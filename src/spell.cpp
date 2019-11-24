#include "spell.hpp"
#include "world.hpp"
#include "bounds.hpp"
#include "entity.hpp"

Spell::Spell(World* world,
             Spell::Type type,
             std::vector<Effect*> effects,
             double cooldown) :
  world(world),
  type(type),
  effects(effects),
  cooldown(cooldown) {
  projectile = {NULL,
                &effects,
                Vec2::zero,
                {20, 0},
                {0, 0}};
}

void Spell::use(Entity* caster, Vec2 target) {
  switch(type) {
  case SELF:
    world->apply_effects(&effects, caster, target);
    break;
  case PROJECTILE: {
    Projectile p = projectile;
    p.source = caster;
    p.position = caster->region->position;
    p.velocity = // projectile.velocity.x *
      Vec2::normalize(target - caster->region->position);
    world->add_projectile(projectile);
  }
    break;
  default:
    break;
  }
}
