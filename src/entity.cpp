#include "entity.hpp"

#include <iostream>

Entity::Entity(double _x, double _y) : _x(_x), _y(_y) {}

double Entity::x() {
  return _x;
}

double Entity::y() {
  return _y;
}

void Entity::x(double _x) {
  this->_x = _x;
}

void Entity::y(double _y) {
  this->_y = _y;
}

void Entity::translate(double dx, double dy) {
  _x += dx;
  _y += dy;
}

std::ostream& operator<<(std::ostream& os, Entity const& e) {
  return os << "(" << e._x << ", " << e._y << ")";
}

MovingEntity::MovingEntity(double _x, double _y) : Entity(_x, _y) {}

Player::Player(double _x, double _y) : MovingEntity(_x, _y) {}

void Player::vx(double _vx) {
  this->_vx = _vx;
}

void Player::vy(double _vy) {
  this->_vy = _vy;
}

void Player::move(double duration) {
  translate(_vx * duration, _vy * duration);
}

Bullet::Bullet(double _x, double _y) : MovingEntity(_x, _y) {}

void Bullet::vx(double _vx) {
  this->_vx = _vx;
}

void Bullet::vy(double _vy) {
  this->_vy = _vy;
}

void Bullet::move(double duration) {
  translate(_vx * duration, _vy * duration);
}
