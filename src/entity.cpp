#include "entity.hpp"

Entity::Entity(double _x, double _y) : _x(_x), _y(_y) {}
double Entity::x() {return _x;}
double Entity::y() {return _y;}
void Entity::move(double dx, double dy) {
  _x += dx;
  _y += dy;
}
