#include "world.hpp"
#include "bounds.hpp"
#include "entity.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <vector>

World::World(Entity *player) : player(player) { spawn(player); }

std::vector<Entity *> World::entities_in_region(Region *region) {
    std::vector<Entity *> out;
    for (Entity *e : entities)
        if (e->is_comp[Entity::OCCUPIES])
            if (region->might_collide(e->occupies.region))
                out.push_back(e);
    return out;
}

void World::spawn(Entity *e) { entities.push_back(e); }

void World::add_projectile(Projectile projectile) {
    projectiles.push_back(projectile);
}

bool World::test_collide(Entity *e, std::vector<Projectile>::iterator p) {
    Vec2 dist = e->occupies.region->position - p->position;
    return Vec2::dot(dist, dist) <
           std::pow(e->occupies.region->bounds()->size().x / 2, 2);
}

/**
 * Returns the maximum hits remaining for the projectile.
 */
int World::apply_projectile_hit(Entity *e, std::vector<Projectile>::iterator p,
                                Timeline *timeline,
                                std::vector<Entity *> *temp_hit) {
    auto p_it = std::find(p->prev_hit.begin(), p->prev_hit.end(), e);
    auto t_it = std::find(temp_hit->begin(), temp_hit->end(), e);
    if (e != p->source && p_it == p->prev_hit.end() &&
        t_it == temp_hit->end()) {
        // we hit an entity!
        p->max_hit -= 1;
        for (Effect *ef : *p->on_hit) {
            ef->targets = {e};
            timeline->add(ef->clone(), 0);
        }
        if (p->max_hit == 0)
            return p->max_hit;
    }
    temp_hit->push_back(e);
    return p->max_hit;
}

void World::move_projectiles(double duration) {
    for (auto p = projectiles.begin(); p != projectiles.end();) {
        // we will do certain things if a projectile has hit a target
        bool hitp = false;
        // if the projectile has a position it wants to travel to,
        // adjust its velocity direction to travel towards that position
        if (p->target != NULL) {
            // THIS IS DONE!
            p->velocity = Vec2::normalize(*p->target - p->position);
            Vec2 l = p->position;
            Vec2 r = p->position + p->velocity * duration * p->speed;
            Vec2 m = *p->target;
            // what is this math? let me explain!
            // l, m, r are collinear
            // that means that (m - l) = lambda * (r - m)
            // case 1: m is in between l and r, lambda > 0
            // case 2: m is outside l and r, lambda < 0
            // case 3: m is at l and r, lambda == 0
            // now! lambda can be found using (m.x - l.x) / (r.x - m.x)
            // however, since we just want the sign, we can multiply instead of
            // divide this also allows us to avoid division errors and cover
            // case 3 with case 1
            if ((r.x - m.x) * (m.x - l.x) >= 0) {
                hitp = true;
                p->position = m;
            } else {
                p->position = r;
            }
        } else {
            p->position = p->position + p->velocity * duration * p->speed;
        }

        // check if it collided with an entity
        std::vector<Entity *> temp_hit;
        if (p->source == player) {
            for (unsigned int i = 0; i < entities.size(); i++) {
                Entity *e = entities[i];
                if (e != player && test_collide(e, p))
                    if (apply_projectile_hit(e, p, &timeline, &temp_hit) == 0)
                        break;
            }
        } else {
            if (test_collide(player, p))
                if (apply_projectile_hit(player, p, &timeline, &temp_hit) == 0)
                    break;
        }
        p->prev_hit = temp_hit;
        if (p->max_hit == 0 || hitp) {
            p = projectiles.erase(p);
        } else {
            p++;
        }
    }
}

void World::clean_up() {
    // moves all entities and projectiles in the world
    for (auto _e = entities.begin(); _e != entities.end();) {
        Entity *e = *_e;
        if (e->is_comp[Entity::LIVES] && e->lives.health <= 0) {
            entities.erase(_e);
            continue;
        }
        if (e->is_comp[Entity::MOVES]) {
            e->move(timeline.diff());
        }
        _e++;
    }
    move_projectiles(timeline.diff());

    // collision detection: player with solids
    bool collided = false;
    for (PolygonRegion *s : *map.solids()) {
        for (Entity *e : entities) {
            if (s->might_collide(e->occupies.region)) {
                Vec2 translation =
                    static_cast<PolygonRegion *>(e->occupies.region)
                        ->min_translate(s);
                if (translation != Vec2::zero) {
                    collided = true;
                    e->occupies.region->position =
                        e->occupies.region->position + translation;
                }
            }
        }
    }
}
