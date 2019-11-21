
#include "entity.hpp"

#include <iostream>
#include "class_util.hpp"

Entity::Entity(Region* region) :
  region(region) {}

void Entity::translate(double dx, double dy) {
  region->position = region->position + Vec2(dx, dy);
}
void Entity::translate(Vec2 v) {
  region->position = region->position + v;
}

std::ostream& operator<<(std::ostream& os, Entity const& e) {
  return os << e.region;
}

MovingEntity::MovingEntity(Region* region) :
  Entity(region) {}

Player::Player(PolygonRegion* region) :
  MovingEntity(region),
  region(region) {}

void Player::move(double duration) {
  translate(velocity * duration);
}

Bullet::Bullet(Region* region) :
  MovingEntity(region) {}

void Bullet::move(double duration) {
  translate(velocity * duration);
}
