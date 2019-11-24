
#include "entity.hpp"

#include <iostream>
#include "class_util.hpp"

Entity::Entity(Region* region) :
  region(region) {}

std::ostream& operator<<(std::ostream& os, Entity const& e) {
  return os << e.region;
}

MovingEntity::MovingEntity(Region* region) :
  Entity(region) {}



Player::Player(PolygonRegion* region, double max_health) :
  MovingEntity(region),
  region(region) {
  this->max_health = max_health;
  this->health = max_health;
}

void Player::move(double duration) {
  region->position = region->position + velocity * duration;
}

Bullet::Bullet(Region* region) :
  MovingEntity(region) {}

void Bullet::move(double duration) {
  region->position = region->position + velocity * duration;
}
