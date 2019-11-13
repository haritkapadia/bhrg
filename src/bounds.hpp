#ifndef BOUNDS_HPP
#define BOUNDS_HPP

#include "class_util.hpp"

struct Position {
  double x;
  double y;
};

class Bounds {
public:
  /**
   * Checks if Bounds contains a point.
   *
   * @param x X-coordinate of the point.
   * @param y Y-coordinate of the point.
   * @return  If Bounds contains the point.
   */
  virtual bool contains(Position self, Position point) = 0;
};

class CircularBounds : public Bounds {
private:
  double _radius;
public:
  GetSetDecl(double, radius);
  CircularBounds(double _radius);
  virtual bool contains(Position self, Position point);
};

class RectangularBounds : public Bounds {
private:
  double _length;
  double _width;
public:
  GetSetDecl(double, length);
  GetSetDecl(double, width);
  RectangularBounds(double _length, double _width);
  virtual bool contains(Position self, Position point);
};

#endif
