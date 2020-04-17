#include "entity.hpp"
#include <iostream>

Entity::Entity() {}

void Entity::move(double duration) {
    if (moves.velocity == Vec2::zero)
        return;
    // std::cout << duration << " - Moved " << name << ": " << moves.speed *
    // duration * moves.velocity << '\n';
    occupies.region->translate(moves.speed * duration * moves.velocity);
}

std::ostream &operator<<(std::ostream &os, Entity const &e) {
    os << "[ ";
    if (e.is_comp[Entity::LIVES]) {
        os << "Lives: " << e.lives << "; ";
    }
    if (e.is_comp[Entity::MOVES]) {
        os << "Moves: " << e.moves << "; ";
    }
    if (e.is_comp[Entity::OCCUPIES]) {
        os << "Occupies: " << e.occupies << "; ";
    }
    if (e.is_comp[Entity::TEXTURED]) {
        os << "Textured: " << e.textured << "; ";
    }
    return os << "]";
}
std::ostream &operator<<(std::ostream &os, Entity::Lives const &l) {
    return os << "alive: " << l.alive << ", "
              << "max_health: " << l.max_health << ", "
              << "health: " << l.health;
}
std::ostream &operator<<(std::ostream &os, Entity::Moves const &l) {
    return os << "velocity: " << l.velocity;
}
std::ostream &operator<<(std::ostream &os, Entity::Occupies const &l) {
    return os << "occupies: " << *l.region;
}

std::ostream &operator<<(std::ostream &os, Entity::Textured const &l) {
    return os << "textured: " << *l.texture;
}

Entity EntityFactory::create() { return e; }
EntityFactory *EntityFactory::lives(Entity::Lives _lives) {
    e.lives = _lives;
    e.is_comp.set(Entity::LIVES);
    return this;
}
EntityFactory *EntityFactory::moves(Entity::Moves _moves) {
    e.moves = _moves;
    e.is_comp.set(Entity::MOVES);
    return this;
}
EntityFactory *EntityFactory::occupies(Entity::Occupies _occupies) {
    e.occupies = _occupies;
    e.is_comp.set(Entity::OCCUPIES);
    return this;
}
EntityFactory *EntityFactory::textured(Entity::Textured _textured) {
    e.textured = _textured;
    e.is_comp.set(Entity::TEXTURED);
    return this;
}
