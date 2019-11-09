#ifndef ENTITY_HPP
#define ENTITY_HPP


class Entity {
private:
  double _x, _y;
public:
  Entity(double, double);
  double x();
  double y();
  void move(double dx, double dy);
};


#endif
