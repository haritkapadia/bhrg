#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "region.hpp"
#include <bitset>
#include <iostream>
#include <ostream>
#include <stack>

class Entity {
  public:
    enum Components { LIVES, MOVES, OCCUPIES, COMP_COUNT };
    struct Lives {
        bool alive;
        double max_health;
        double health;
        void damage(double dh) {
            health -= dh;
            if (health < 0)
                alive = false;
        }
        void heal(double dh) {
            health += dh;
            if (health > max_health)
                health = max_health;
            else if (health < 0)
                alive = false;
        }
        void kill() {
            health = 0;
            alive = false;
        }
    };
    struct Moves {
        Vec2 velocity;
        double speed;
    };
    struct Occupies {
        Region *region;
    };
    std::bitset<Entity::COMP_COUNT> is_comp;
    std::string name = "Entity";
    Lives lives;
    Moves moves;
    Occupies occupies;
    Entity();
    Entity(Entity const &e);
    void move(double duration);
    friend std::ostream &operator<<(std::ostream &os, Entity const &e);
    friend std::ostream &operator<<(std::ostream &os, Lives const &l);
    friend std::ostream &operator<<(std::ostream &os, Moves const &l);
    friend std::ostream &operator<<(std::ostream &os, Occupies const &l);
};

class EntityFactory {
  public:
    Entity e;
    EntityFactory *lives(Entity::Lives _lives);
    EntityFactory *moves(Entity::Moves _moves);
    EntityFactory *occupies(Entity::Occupies _occupies);
    Entity create();
};

#endif
