#ifndef BOUNDS_HPP
#define BOUNDS_HPP

#include <vector>
#include "class_util.hpp"
#include "vec.hpp"

/**
 * A virtual class describing a shape of a bounded space.
 */
class Bounds {
public:
  /**
   * Scales the bounds from the center by a factor.
   *
   * @param factor   The multiplicative factor by which to scale the bounds.
   */
  virtual void scale(double factor) = 0;
  /**
   * Gets the size of the axis-aligned bounding box of the Bounds.
   *
   * @return A 2D vector with the width and height of the box.
   */
  virtual Vec2 size() = 0;
};

/**
 * A circular bounded space.
 */
class CircularBounds : public Bounds {
private:
  double _radius;
public:
  /**
   * @param _radius   The radius of the circle.
   */
  CircularBounds(double _radius);
  /**
   * @return The radius of the circle.
   */
  double radius();
  /**
   * @param _radius   The new radius of the circle.
   */
  void radius(double _radius);
  virtual void scale(double factor);
  virtual Vec2 size();
};

/**
 * A virtual class describing a polygon bounded space.
 */
class PolygonBounds : public Bounds {
public:
  /**
   * @return The vertices of the polygon as vectors originating from the center.
   */
  virtual std::vector<Vec2> vertices() = 0;
  /**
   * @return The normal vectors of the polygon's faces, facing outward.
   */
  virtual std::vector<Vec2> normals() = 0;
  virtual void scale(double factor) = 0;
  virtual Vec2 size() = 0;
};

/**
 * A rectangular bounded space.
 */
class RectangularBounds : public PolygonBounds {
private:
  Vec2 _size;
public:
  /**
   * @param _size   The width and height of the rectangle.
   */
  RectangularBounds(Vec2 _size);
  /**
   * @param _size  The new width and height of the rectangle.
   */
  void size(Vec2 _size);
  virtual std::vector<Vec2> vertices();
  virtual std::vector<Vec2> normals();
  virtual void scale(double factor);
  virtual Vec2 size();
};

/**
 * A convex polygon bounded space.
 */
class ConvexBounds : public PolygonBounds {
private:
  std::vector<Position> _vertices;
  Vec2 _size;
public:
  /**
   * @param _vertices   The vertices of the polygon in counterclockwise order.
   * @param _size       The bounding box of the polygon.
   */
  ConvexBounds(std::vector<Position> _vertices, Vec2 _size);
  /**
   * @return The number of vertices.
   */
  unsigned int count();
  /**
   * @param _vertices   The new vertices of the polygon.
   */
  virtual void vertices(std::vector<Vec2> _vertices);
  virtual std::vector<Vec2> vertices();
  virtual std::vector<Vec2> normals();
  virtual void scale(double factor);
  virtual Vec2 size();
};

#endif
