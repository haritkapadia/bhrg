#ifndef BOUNDS_HPP
#define BOUNDS_HPP

#include "class_util.hpp"
#include "vec.hpp"
#include <vector>

class Bounds {
public:
  virtual bool contains(Position self, Position point) = 0;
  virtual void scale(double factor) = 0;
  virtual Vec2 size() = 0;
};

class CircularBounds : public Bounds {
private:
  double _radius;
public:
  CircularBounds(double _radius);
  GetSetDecl(double, radius);
  virtual bool contains(Position self, Position point);
  virtual void scale(double factor);
  virtual Vec2 size();
};

class PolygonBounds : public Bounds {
public:
  virtual bool contains(Position self, Position point) = 0;
  virtual void scale(double factor) = 0;
  virtual Vec2 size() = 0;
  virtual std::vector<Vec2> vertices() = 0;
  virtual std::vector<Vec2> normals() = 0;
};

class RectangularBounds : public PolygonBounds {
private:
  Vec2 _size;
public:
  RectangularBounds(Vec2 _size);
  virtual bool contains(Position self, Position point);
  virtual void scale(double factor);
  virtual GetDecl(Vec2, size);
  SetDecl(Vec2, size);
  virtual std::vector<Vec2> vertices();
  virtual std::vector<Vec2> normals();
};

class ConvexBounds : public PolygonBounds {
private:
  std::vector<Position> _vertices;
  Vec2 _size;
public:
  ConvexBounds(std::vector<Position> _vertices, Vec2 _size);
  virtual bool contains(Position self, Position point);
  virtual void scale(double factor);
  virtual GetDecl(Vec2, size);
  GetSetDecl(std::vector<Position>, vertices);
  virtual std::vector<Vec2> normals();
  unsigned int count();
};

#endif
