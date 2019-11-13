#include "bounds.hpp"
#include <cmath>

#define Inside(L, N, R) ((L) < (N) && (N) < (R))
#define Pythagorean(A, B, C) ((A)*(A) + (B)*(B) < (C)*(C))

CircularBounds::CircularBounds(double _radius) :
  Bounds(),
  _radius(_radius) {}

GetSet(CircularBounds, double, radius);

bool CircularBounds::contains(Position self, Position other) {
  return Pythagorean(other.x - self.x, other.y - self.y, _radius);
}

RectangularBounds::RectangularBounds(double _length, double _width) :
  Bounds(),
  _length(_length),
  _width(_width) {}

GetSet(RectangularBounds, double, length);
GetSet(RectangularBounds, double, width);

bool RectangularBounds::contains(Position self, Position other) {
  return
    Inside(self.x, other.x, self.x + _length) &&
    Inside(self.y, other.y, self.y + _width);
}

#undef Inside
#undef Pythagorean
