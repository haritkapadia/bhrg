#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <ostream>
#include "bounds.hpp"
#include "class_util.hpp"
#include "vec.hpp"
#include "region.hpp"

class Entity {
public:
  Region* region;
  Entity(Region* region);
  void translate(double dx, double dy);
  void translate(Vec2 v);
  friend std::ostream& operator<<(std::ostream& os, Entity const& e);
};

class MovingEntity : public Entity {
public:
  MovingEntity(Region* region);
  virtual void move(double duration) = 0;
};

class Player : public MovingEntity {
public:
  PolygonRegion* region;
  Player(PolygonRegion* region);
  Vec2 velocity;
  virtual void move(double duration);
};

class Bullet : public MovingEntity {
public:
  Bullet(Region* region);
  Vec2 velocity;
  virtual void move(double duration);
};


#endif
