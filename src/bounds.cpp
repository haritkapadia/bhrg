
#include "bounds.hpp"
#include <cmath>

// checks if N is inside the open interval (L, R)
#define Inside(L, N, R) ((L) < (N) && (N) < (R))
// checks if A, B, C (A, B < C) is a possible triangle
// budget triangle inequality
#define Pythagorean(A, B, C) ((A)*(A) + (B)*(B) < (C)*(C))

// empty constructor
CircularBounds::CircularBounds(double _radius) :
  Bounds(),
  _radius(_radius) {}

GetSet(CircularBounds, double, radius);

void CircularBounds::scale(double factor) {
  _radius *= factor;
}
Vec2 CircularBounds::size() {
  return Vec2(_radius, _radius) * 2;
}



RectangularBounds::RectangularBounds(Vec2 _size) :
  PolygonBounds(),
  _size(_size) {}

void RectangularBounds::scale(double factor) {
  _size = _size * factor;
}

GetSet(RectangularBounds, Vec2, size);

std::vector<Vec2> RectangularBounds::vertices() {
  return {_size * -0.5,
          Vec2::cross(_size) * -0.5,
          Vec2::cross(_size) * 0.5,
          _size * 0.5};
}

std::vector<Vec2> RectangularBounds::normals() {
  return {{Vec2(1, 0), Vec2(0, 1)}};
}

ConvexBounds::ConvexBounds(std::vector<Position> _vertices, Vec2 _size) :
  PolygonBounds(),
  _vertices(_vertices),
  _size(_size) {}

void ConvexBounds::scale(double factor) {
  // temporarily unimplemented
}

Get(ConvexBounds, Vec2, size);

GetSet(ConvexBounds, std::vector<Position>, vertices);

unsigned int ConvexBounds::count() {
  return _vertices.size();
}

std::vector<Vec2> ConvexBounds::normals() {
  std::vector<Vec2> out;
  // makes each normal by rotating a vector created from two vertices
  Vec2 v0v1 = _vertices[0] - _vertices[_vertices.size() - 1];
  out.push_back(Vec2::normalize(Vec2::cross(v0v1)));
  for(unsigned int i = 1; i < _vertices.size(); i++) {
    Vec2 v0v1 = _vertices[i] - _vertices[i - 1];
    out.push_back(Vec2::normalize(Vec2::cross(v0v1)));
  }
  return out;
}

#undef Inside
#undef Pythagorean
