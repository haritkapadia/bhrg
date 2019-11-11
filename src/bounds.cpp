#include "bounds.hpp"

#define Inside(L, N, R) ((L) < (N) && (N) < (R))

Bounds::Bounds(double _x, double _y, double _length, double _width) : _x(_x), _y(_y), _length(_length), _width(_width) {}

bool Bounds::contains(double x, double y) {
  return Inside(_x, x, _x + _length) && Inside(_y, y, _y + _width);
}

#undef Inside
