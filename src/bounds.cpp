
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

void CircularBounds::scale(double factor) {
  _radius *= factor;
}
Vec2 CircularBounds::size() {
  return Vec2(_radius, _radius) * 2;
}



RectangularBounds::RectangularBounds(Vec2 _size) :
  PolygonBounds(),
  _size(_size) {}

bool RectangularBounds::contains(Position self, Position other) {
  return
    Inside(self.x - _size.x / 2, other.x, self.x + _size.x / 2) &&
    Inside(self.y - _size.y / 2, other.y, self.y + _size.y / 2);
}

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

bool ConvexBounds::contains(Position self, Position other) {
  return false;
}

void ConvexBounds::scale(double factor) {
}

Get(ConvexBounds, Vec2, size);

GetSet(ConvexBounds, std::vector<Position>, vertices);

unsigned int ConvexBounds::count() {
  return _vertices.size();
}

std::vector<Vec2> ConvexBounds::normals() {
  std::vector<Vec2> out;
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
