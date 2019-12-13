#ifndef EFFECT_HPP
#define EFFECT_HPP

#include "entity.hpp"
#include "timeline.hpp"

class Effect : public Event {
public:
  int pulses = 0;
  int last_pulse = 0;
  Vec2 point = Vec2::zero;
  std::vector<Entity*> targets;
  Effect(std::vector<Entity*> targets, double duration);
  virtual void act(double progress);
  virtual void start(Entity* e) = 0;
  virtual void stop(Entity* e) = 0;
  virtual void pulse(Entity* e) = 0;
  virtual Effect* clone() = 0;
};

class DamageOverTime : public Effect {
public:
  DamageOverTime(std::vector<Entity*> targets, double duration);
  virtual void start(Entity* e);
  virtual void stop(Entity* e);
  virtual void pulse(Entity* e);
  virtual DamageOverTime* clone();
};

class Speed : public Effect {
private:
  double mod;
public:
  Speed(std::vector<Entity*> targets, double duration, double mod);
  virtual void start(Entity* e);
  virtual void stop(Entity* e);
  virtual void pulse(Entity* e);
  virtual Speed* clone();
};

class Teleport : public Effect {
public:
  Teleport(std::vector<Entity*> targets);
  virtual void start(Entity* e);
  virtual void stop(Entity* e);
  virtual void pulse(Entity* e);
  virtual Teleport* clone();
};

#endif
