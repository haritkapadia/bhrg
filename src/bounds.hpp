#ifndef BOUNDS_HPP
#define BOUNDS_HPP

class Bounds {
private:
  double _x;
  double _y;
  double _length;
  double _width;
public:
  Bounds(double _x, double _y, double _length, double _width);
  bool contains(double x, double y);
};

#endif
