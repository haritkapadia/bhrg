#ifndef SOLID_HPP
#define SOLID_HPP

#include "vec.hpp"
#include "bounds.hpp"
#include <algorithm>

class Region {
private:
  Bounds* _bounds;
public:
  Position position;
  Region(Position position, Bounds* bounds);
  virtual Bounds* bounds();
  bool might_collide(Region* other);
  friend std::ostream& operator<<(std::ostream& os, Region const& e);
};

class PolygonRegion : public Region {
private:
  Position translate_vertex(Position vertex);
public:
  PolygonRegion(Position position, PolygonBounds* bounds);
  PolygonBounds* bounds();
  std::vector<Position> vertices();
  std::pair<double, double> project(Vec2 axis);
  bool collides(PolygonRegion* other);
  Vec2 min_translate(PolygonRegion* solid);
};

#endif
