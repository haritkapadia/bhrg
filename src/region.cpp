#include "region.hpp"
#include <algorithm>
#include <functional>

Region::Region(Position position, Bounds* bounds) :
  position(position),
  _bounds(bounds) {}

Bounds* Region::bounds() {
  return _bounds;
}

#define Inside(L, N, R) ((L) < (N) && (N) < (R))
bool Region::might_collide(Region* other) {
  double al = this->position.x - this->bounds()->size().x / 2;
  double ar = this->position.x + this->bounds()->size().x / 2;
  double ad = this->position.y - this->bounds()->size().y / 2;
  double au = this->position.y + this->bounds()->size().y / 2;
  double bl = other->position.x - other->bounds()->size().x / 2;
  double br = other->position.x + other->bounds()->size().x / 2;
  double bd = other->position.y - other->bounds()->size().y / 2;
  double bu = other->position.y + other->bounds()->size().y / 2;
  return (Inside(al, bl, ar) || Inside(al, br, ar)) &&
    (Inside(ad, bd, au) || Inside(ad, bu, au));
}

PolygonRegion::PolygonRegion(Position position, PolygonBounds* bounds) :
  Region(position, bounds) {}

Position PolygonRegion::translate_vertex(Position vertex) {
  return {vertex.x + position.x, vertex.y + position.y};
}

std::vector<Position> PolygonRegion::vertices() {
  std::vector<Position> out = bounds()->vertices();
  std::transform(out.begin(), out.end(), out.begin(),
                 std::bind(&PolygonRegion::translate_vertex, this, std::placeholders::_1));
  return out;
}

std::pair<double, double> PolygonRegion::project(Vec2 axis) {
  double min = std::numeric_limits<double>::max();
  double max = -std::numeric_limits<double>::max();
  for(Position v : vertices()) {
    double d = Vec2::dot(v, axis);
    if(d < min)
      min = d;
    if(d > max)
      max = d;
  }
  return std::make_pair(min, max);
}

PolygonBounds* PolygonRegion::bounds() {
  return static_cast<PolygonBounds*>(Region::bounds());
}

bool PolygonRegion::collides(PolygonRegion* other) {
  std::vector<Vec2> a_axes = this->bounds()->normals();
  std::vector<Vec2> b_axes = other->bounds()->normals();
  for(std::vector<Vec2> axes : {a_axes, b_axes}) {
    for(Vec2 axis : axes) {
      std::pair<double, double> lra = this->project(axis);
      std::pair<double, double> lrb = other->project(axis);
      if(!(Inside(lra.first, lrb.first, lra.second) ||
           Inside(lra.first, lrb.second, lra.second)))
        return false;
    }
  }
  return true;
}

Vec2 PolygonRegion::min_translate(PolygonRegion* solid) {
  std::vector<Vec2> a_axes = solid->bounds()->normals();
  std::vector<Vec2> b_axes = this->bounds()->normals();
  Vec2 min_axis;
  double min_overlap = std::numeric_limits<double>::max();
  for(std::vector<Vec2> axes : {a_axes, b_axes}) {
    for(Vec2 axis : axes) {
      std::pair<double, double> lra = solid->project(axis);
      std::pair<double, double> lrb = this->project(axis);
      double overlap = 0;
      if(Inside(lra.first, lrb.first, lra.second))
        overlap = lra.second - lrb.first;
      else if(Inside(lra.first, lrb.second, lra.second))
        overlap = lrb.second - lra.first;
      else
        return Vec2(0, 0);
      if(overlap < min_overlap) {
        min_overlap = overlap;
        min_axis = axis;
      }
    }
  }
  Vec2 vcenter = this->position - solid->position;
  double scalar_proj = Vec2::dot(vcenter, min_axis) / Vec2::dot(vcenter, vcenter);
  char direction = scalar_proj > 0 ? 1 : -1;
  min_axis = Vec2::times(min_axis, direction * min_overlap);
  return min_axis;
}

#undef Inside



std::ostream& operator<<(std::ostream& os, Region const& e) {
  return os << "(" << e.position.x << ", " << e.position.y << ")";
}
