#ifndef VEC_HPP
#define VEC_HPP

#include <ostream>

/**
 * A 2D math vector.
 */
class Vec2 {
public:
  double x;
  double y;
  Vec2(double x, double y);
  Vec2(const Vec2 &v);
  Vec2();
  static const Vec2 zero;
  static double dot(Vec2 v1, Vec2 v2);
  /**
   * @param v   The vector of which to find the perpendicular from.
   * @return A vector perpendicular to every vector given.
   */
  static Vec2 cross(Vec2 v);
  static Vec2 times(Vec2 v, double s);
  static Vec2 times(double s, Vec2 v);
  static Vec2 normalize(Vec2 v);
  static Vec2 add(Vec2 v1, Vec2 v2);
  static Vec2 subtract(Vec2 v1, Vec2 v2);
  static bool equal(Vec2 v1, Vec2 v2);
  static double length(Vec2 v);
  friend Vec2 operator*(const Vec2& v2, const double s);
  friend Vec2 operator*(const double s, const Vec2& v2);
  friend Vec2 operator+(const Vec2& v1, const Vec2& v2);
  friend Vec2 operator-(const Vec2& v1, const Vec2& v2);
  friend bool operator==(const Vec2& v1, const Vec2& v2);
  friend bool operator!=(const Vec2& v1, const Vec2& v2);
  friend std::ostream& operator<<(std::ostream& os, Vec2 const& e);
};

typedef Vec2 Position;

#endif
