#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <ostream>
#include "bounds.hpp"
#include "class_util.hpp"
#include "vec.hpp"
#include "region.hpp"

/**
 * A unique object in the world occupying a region and containing special data.
 */
class Entity {
public:
  Region* region;
  bool alive = false;
  double max_health = 0;
  double health = 0;
  /**
   * @param region   The region that the entity occupies.
   */
  Entity(Region* region);
  friend std::ostream& operator<<(std::ostream& os, Entity const& e);
};

/**
 * An entity that moves based on time.
 */
class MovingEntity : public Entity {
public:
  MovingEntity(Region* region);
  /**
   * Moves the entity based on how a duration of time.
   *
   * @param duration   The duration of time for which the entity moves.
   */
  virtual void move(double duration) = 0;
};


class Player : public MovingEntity {
public:
  PolygonRegion* region;
  Vec2 velocity;
  /**
   * @param region   The region that the player occupies.
   */
  Player(PolygonRegion* region, double max_health);
  virtual void move(double duration);
};



class Bullet : public MovingEntity {
public:
  Vec2 velocity;
  /**
   * @param region   The region that the bullet occupies.
   */
  Bullet(Region* region);
  virtual void move(double duration);
};

#endif
