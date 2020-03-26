#include "spell.hpp"
#include <algorithm>
#include <functional>

void Spell::use(World *world) {
    switch (type) {
    case SELF:
        for (Effect *e : effects) {
            world->timeline.add(e->clone(), 0);
        }
        break;
    case POINT_TARGET:
        for (Effect *e : effects) {
            e->point = region->position;
            world->timeline.add(e->clone(), 0);
        }
        break;
    case ENTITY_TARGET:
        for (Effect *e : effects) {
            e->targets = {target};
            world->timeline.add(e->clone(), 0);
        }
        break;
    case REGION: {
        std::vector<Entity *> targets = world->entities_in_region(region);
        for (Effect *e : effects) {
            e->targets = targets;
            world->timeline.add(e->clone(), 0);
        }
    } break;
    case PROJECTILE: {
        Projectile p;
        p.source = source;
        p.position = source->occupies.region->position;
        p.velocity = Vec2::normalize(region->position -
                                     source->occupies.region->position);
        p.speed = 15;
        p.max_hit = 1;
        p.on_hit = &effects;
        // p.target = new Vec2(-5, 5);
        world->add_projectile(p);
    } break;
    default:
        break;
    }
}
