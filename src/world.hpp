#ifndef WORLD_HPP
#define WORLD_HPP

#include "bounds.hpp"
#include "effect.hpp"
#include "entity.hpp"
#include "map.hpp"
#include <functional>
#include <set>
#include <unordered_set>
#include <vector>

struct Projectile {
    Entity *source;
    Position position;
    // this is actually just a direction, it is then multiplied by speed
    Vec2 velocity;
    double speed;
    // if target is non-null, velocity will be adjusted
    // to point towards the target based on speed
    Vec2 *target = NULL;
    int max_hit = 1;
    std::vector<Effect *> *on_hit;
    // this should probably be a set, but i keep getting segfaults when using
    // sets :(
    std::vector<Entity *> prev_hit = {};
};

// class HitCooldown : public Event {
// public:
//   unsigned int e_idx;
//   Projectile* p;
//   int i;
//   HitCooldown(unsigned int e_idx, Projectile* p, int i);
//   virtual void act(double progress);
// };

class World {
  private:
    bool test_collide(Entity *e, std::vector<Projectile>::iterator p);
    int apply_projectile_hit(Entity *e, std::vector<Projectile>::iterator p,
                             Timeline *timeline,
                             std::vector<Entity *> *temp_hit);

  public:
    World(Entity *player);
    Entity *player;
    std::vector<Entity *> entities;
    std::vector<Entity *> entities_in_region(Region *region);
    void spawn(Entity *entity);
    void add_projectile(Projectile projectile);
    void move_projectiles(double duration);
    void clean_up();
    std::vector<Projectile> projectiles;
    Map map;
    Timeline timeline;
};

#endif
