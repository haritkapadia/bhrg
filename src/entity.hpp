#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <ostream>
#include "bounds.hpp"
#include "class_util.hpp"

class Entity {
private:
  Position _position;
  Bounds* _bounds;
public:
  Entity(Position _position, Bounds* _bounds);
  GetSetDecl(Position, position);
  GetSetDecl(Bounds*, bounds);
  void translate(double dx, double dy);
  friend std::ostream& operator<<(std::ostream& os, Entity const& e);
};

class MovingEntity : public Entity {
public:
  MovingEntity(Position _position, Bounds* _bounds);
  virtual void move(double duration) = 0;
};

class Player : public MovingEntity {
private:
  double _vx;
  double _vy;
public:
  Player(Position _position, Bounds* _bounds);
  GetSetDecl(double, vx);
  GetSetDecl(double, vy);
  virtual void move(double duration);
};

class Bullet : public MovingEntity {
private:
  double _vx;
  double _vy;
public:
  Bullet(Position _position, Bounds* _bounds);
  GetSetDecl(double, vx);
  GetSetDecl(double, vy);
  virtual void move(double duration);
};


#endif
