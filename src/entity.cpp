#include "entity.hpp"

#include <iostream>
#include "class_util.hpp"

Entity::Entity(Position _position, Bounds* _bounds) :
  _position(_position),
  _bounds(_bounds) {}

GetSet(Entity, Position, position);
GetSet(Entity, Bounds*, bounds);

void Entity::translate(double dx, double dy) {
  _position.x += dx;
  _position.y += dy;
}

std::ostream& operator<<(std::ostream& os, Entity const& e) {
  return os << "(" << e._position.x << ", " << e._position.y << ")";
}

MovingEntity::MovingEntity(Position _position, Bounds* _bounds) :
  Entity(_position, _bounds) {}

Player::Player(Position _position, Bounds* _bounds) :
  MovingEntity(_position, _bounds) {}

GetSet(Player, double, vx);
GetSet(Player, double, vy);

void Player::move(double duration) {
  translate(_vx * duration, _vy * duration);
}

Bullet::Bullet(Position _position, Bounds* _bounds) :
  MovingEntity(_position, _bounds) {}

GetSet(Bullet, double, vx);
GetSet(Bullet, double, vy);

void Bullet::move(double duration) {
  translate(_vx * duration, _vy * duration);
}
