#include "effect.hpp"
#include <vector>

Effect::Effect(std::vector<Entity*> targets, double duration) :
    Event(0, duration),
    targets(targets) {}

void Effect::act(double progress) {
  if(progress == 0)
    for(Entity* e : targets)
      start(e);
  else if(progress == 1)
    for(Entity* e : targets)
      stop(e);
  else
    for(; last_pulse < progress * pulses; last_pulse++)
      for(Entity* e : targets)
        pulse(e);
}

DamageOverTime::DamageOverTime(std::vector<Entity*> targets, double duration) :
    Effect(targets, duration) {
  pulses = 50;
}

DamageOverTime* DamageOverTime::clone() {
  DamageOverTime* other = new DamageOverTime(targets, duration);
  other->pulses = pulses;
  other->last_pulse = last_pulse;
  return other;
}

void DamageOverTime::start(Entity* e) {}
void DamageOverTime::pulse(Entity* e) {
  e->lives.damage(1);
}
void DamageOverTime::stop(Entity* e) {}




Speed::Speed(std::vector<Entity*> targets, double duration) :
    Effect(targets, duration) {
  pulses = 50;
}

Speed* Speed::clone() {
  Speed* other = new Speed(targets, duration);
  other->pulses = pulses;
  other->last_pulse = last_pulse;
  return other;
}

void Speed::start(Entity* e) {
  e->moves.speed *= 1.5;
}

void Speed::pulse(Entity* e) {}
void Speed::stop(Entity* e) {
  e->moves.speed /= 1.5;
}


Teleport::Teleport(std::vector<Entity*> targets) :
    Effect(targets, 0) {}

Teleport* Teleport::clone() {
  Teleport* other = new Teleport(targets);
  other->target = target;
  other->pulses = pulses;
  other->last_pulse = last_pulse;
  return other;
}

void Teleport::start(Entity* e) {
  e->occupies.region->position = target;
}

void Teleport::pulse(Entity* e) {}
void Teleport::stop(Entity* e) {}
