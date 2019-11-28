#include "entity.hpp"
#include <iostream>



std::ostream& operator<<(std::ostream& os, Entity const& e) {
  os << "[ ";
  if(e.is_comp[Entity::LIVES]) {
    os << "Lives: " << e.lives << "; ";
  }
  if(e.is_comp[Entity::MOVES]) {
    os << "Moves: " << e.moves << "; ";
  }
  if(e.is_comp[Entity::OCCUPIES]) {
    os << "Occupies: " << e.occupies << "; ";
  }
  return os << "]";
}
std::ostream& operator<<(std::ostream& os, Entity::Lives const& l) {
  return os << "alive: " << l.alive << ", "
            << "max_health: " << l.max_health << ", "
            << "health: " << l.health;
}
std::ostream& operator<<(std::ostream& os, Entity::Moves const& l) {
  return os << "velocity: " << l.velocity;
}
std::ostream& operator<<(std::ostream& os, Entity::Occupies const& l) {
  return os << "occupies: " << *l.region;
}


Entity EntityFactory::create() {
  return e;
}
EntityFactory* EntityFactory::lives(Entity::Lives _lives) {
  e.lives = _lives;
  e.is_comp.set(Entity::LIVES);
  return this;
}
EntityFactory* EntityFactory::moves(Entity::Moves _moves) {
  e.moves = _moves;
  e.is_comp.set(Entity::MOVES);
  return this;
}
EntityFactory* EntityFactory::occupies(Entity::Occupies _occupies) {
  e.occupies = _occupies;
  e.is_comp.set(Entity::OCCUPIES);
  return this;
}
