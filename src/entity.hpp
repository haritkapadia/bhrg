#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <ostream>

class Entity {
private:
  double _x, _y;
public:
  Entity(double _x, double _y);
  double x();
  double y();
  void x(double _x);
  void y(double _y);
  void translate(double dx, double dy);
  friend std::ostream& operator<<(std::ostream& os, Entity const& e);
};

class MovingEntity : public Entity {
public:
  MovingEntity(double _x, double _y);
  virtual void move(double duration) = 0;
};

class Player : public MovingEntity {
private:
  double _vx;
  double _vy;
public:
  Player(double _x, double _y);
  void vx(double _vx);
  void vy(double _vy);
  virtual void move(double duration);
};

class Bullet : public MovingEntity {
private:
  double _vx;
  double _vy;
public:
  Bullet(double _x, double _y);
  void vx(double _vx);
  void vy(double _vy);
  virtual void move(double duration);
};


#endif
