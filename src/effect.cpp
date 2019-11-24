#include "effect.hpp"


void Teleport::affect(Entity* dest, Vec2 target) {
  dest->region->position = target;
}

Damage::Damage(double damage) : damage(damage) {}
void Damage::affect(Entity* dest, Vec2 target) {
  dest->health -= damage;
}
