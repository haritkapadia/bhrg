#ifndef EFFECT_HPP
#define EFFECT_HPP

#include "entity.hpp"
#include "timeline.hpp"

class Effect : public Event {
public:
  int pulses = 0;
  int last_pulse = 0;
  std::vector<Entity*> targets;
  Effect(std::vector<Entity*> targets, double duration);
  virtual void act(double progress);
  virtual void start(Entity* e) = 0;
  virtual void stop(Entity* e) = 0;
  virtual void pulse(Entity* e) = 0;
};

class DamageOverTime : public Effect {
public:
  DamageOverTime(std::vector<Entity*> targets, double duration);
  virtual void start(Entity* e);
  virtual void stop(Entity* e);
  virtual void pulse(Entity* e);
  DamageOverTime* clone();
};

class Speed : public Effect {
public:
  Speed(std::vector<Entity*> targets, double duration);
  virtual void start(Entity* e);
  virtual void stop(Entity* e);
  virtual void pulse(Entity* e);
  Speed* clone();
};

class Teleport : public Effect {
public:
  Vec2 target;
  Teleport(std::vector<Entity*> targets);
  virtual void start(Entity* e);
  virtual void stop(Entity* e);
  virtual void pulse(Entity* e);
  Teleport* clone();
};

#endif
