#include "vec.hpp"
#include <cmath>
#include <ostream>

Vec2::Vec2(double x, double y) : x(x), y(y) {}
Vec2::Vec2(const Vec2 &v) : x(v.x), y(v.y) {}
Vec2::Vec2() : x(0), y(0) {}
const Vec2 Vec2::zero = Vec2();

double Vec2::dot(Vec2 v1, Vec2 v2) { return v1.x * v2.x + v1.y * v2.y; }
Vec2 Vec2::cross(Vec2 v) {
    // Equivalent to solving 2D determinant |[i, j] , [v_x, v_y]|
    return Vec2(v.y, -v.x);
}
Vec2 Vec2::times(Vec2 v, double s) { return Vec2(v.x * s, v.y * s); }
Vec2 Vec2::times(double s, Vec2 v) { return Vec2(v.x * s, v.y * s); }
Vec2 Vec2::normalize(Vec2 v) {
    double length = std::sqrt(Vec2::dot(v, v));
    if (length == 0)
        return Vec2::zero;
    else
        return Vec2(v.x / length, v.y / length);
}
Vec2 Vec2::add(Vec2 v1, Vec2 v2) { return Vec2(v1.x + v2.x, v1.y + v2.y); }
Vec2 Vec2::subtract(Vec2 v1, Vec2 v2) { return Vec2(v1.x - v2.x, v1.y - v2.y); }
bool Vec2::equal(Vec2 v1, Vec2 v2) { return v1.x == v2.x && v1.y == v2.y; }
double Vec2::length(Vec2 v) { return std::sqrt(v.x * v.x + v.y * v.y); }

Vec2 operator*(const Vec2 &v1, const double s) { return Vec2::times(v1, s); }
Vec2 operator*(const double s, const Vec2 &v1) { return Vec2::times(s, v1); }
Vec2 operator+(const Vec2 &v1, const Vec2 &v2) { return Vec2::add(v1, v2); }
Vec2 operator-(const Vec2 &v1, const Vec2 &v2) { return Vec2::subtract(v1, v2); }
bool operator==(const Vec2 &v1, const Vec2 &v2) { return Vec2::equal(v1, v2); }
bool operator!=(const Vec2 &v1, const Vec2 &v2) { return !Vec2::equal(v1, v2); }
std::ostream &operator<<(std::ostream &os, Vec2 const &e) {
    return os << "Vec2[" << e.x << ' ' << e.y << "]";
}
