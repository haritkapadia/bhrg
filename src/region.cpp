#include "region.hpp"
#include "debug.hpp"
#include "prettyprint.hpp"
#include <cmath>
#include <limits>
#include <vector>

#define Inside(L, N, R) ((L) < (N) && (N) < (R))

bool Region::might_collide(Region *a, Region *b) {
    Vec2 a_p = a->bottom_left(), a_s = a->size();
    Vec2 b_p = b->bottom_left(), b_s = b->size();
    double al = a_p.x;
    double ar = a_p.x + a_s.x;
    double ad = a_p.y;
    double au = a_p.y + a_s.y;
    double bl = b_p.x;
    double br = b_p.x + b_s.x;
    double bd = b_p.y;
    double bu = b_p.y + b_s.y;
    return (Inside(al, bl, ar) || Inside(al, br, ar)) && (Inside(ad, bd, au) || Inside(ad, bu, au));
}

/*
  Here is a list of what collision is stored in what class:
  Circle
  Convex
  Rectangle : public Convex
  Circle->Circle: Circle
  Circle->Convex: Convex
  Convex->Convex: Convex
  Convex->Circle: Convex
 */

bool Region::collide(Region *a, Region *b) {
    if (auto _a = dynamic_cast<Circle *>(a)) {
        if (auto _b = dynamic_cast<Circle *>(b)) {
            return _a->collide(_b);
        }
        if (auto _b = dynamic_cast<Convex *>(b)) {
            return _b->collide(_a);
        }
    }
    if (auto _a = dynamic_cast<Convex *>(a)) {
        if (auto _b = dynamic_cast<Circle *>(b)) {
            return _a->collide(_b);
        }
        if (auto _b = dynamic_cast<Convex *>(b)) {
            return _a->collide(_b);
        }
    }
    return false;
}

Vec2 Region::uncollide(Region *r, Region *s) {
    if (auto _r = dynamic_cast<Circle *>(r)) {
        if (auto _s = dynamic_cast<Circle *>(s)) {
            return _r->uncollide(_s);
        }
        if (auto _s = dynamic_cast<Convex *>(s)) {
            return -1 * _s->uncollide(_r);
        }
    }
    if (auto _r = dynamic_cast<Convex *>(r)) {
        if (auto _s = dynamic_cast<Circle *>(s)) {
            return _r->uncollide(_s);
        }
        if (auto _s = dynamic_cast<Convex *>(s)) {
            return _r->uncollide(_s);
        }
    }
    debug::print("How did I get here under Region::uncollide?");
    return Vec2::zero;
}

std::ostream &operator<<(std::ostream &os, Region const &r) { return r.print(os); }

/////////

Circle::Circle(Vec2 center, double radius) : _center(center), radius(radius) {}

void Circle::scale(double factor, Vec2 anchor) {
    radius *= factor;
    _center = (_center - anchor) + _center;
}

void Circle::translate(Vec2 d) { _center = _center + d; }

void Circle::move_to(Vec2 p) { _center = p; }

Vec2 Circle::size() { return Vec2(2 * radius, 2 * radius); }

Vec2 Circle::bottom_left() { return _center - Vec2(radius, radius); }

Vec2 Circle::center() { return _center; }

bool Circle::contains(Vec2 p) {
    Vec2 dist = _center - p;
    return Vec2::dot(dist, dist) < radius * radius;
}

bool Circle::collide(Circle *a) {
    return Vec2::length(_center - a->center()) <= radius + a->radius;
}

// TODO this might be a wrong, we'll find out!!!
Vec2 Circle::uncollide(Circle *s) {
    Vec2 rs = Vec2::normalize(s->center() - _center);
    // s->radius * norm(r - s) - radius * norm(s - r)
    double translation = Vec2::length(_center - s->center()) - radius - s->radius;
    if (translation < 0)
        return translation * rs;
    else
        return Vec2::zero;
}

std::ostream &Circle::print(std::ostream &os) const {
    return os << "Circle[" << _center << ' ' << radius << "]";
}

/////////

Convex::Convex(std::vector<Vec2> vertices) {
    double min_x = std::numeric_limits<double>::infinity();
    double min_y = std::numeric_limits<double>::infinity();
    double max_x = -std::numeric_limits<double>::infinity();
    double max_y = -std::numeric_limits<double>::infinity();
    for (auto v = vertices.begin(); v != vertices.end(); v++) {
        if (v->x < min_x)
            min_x = v->x;
        if (v->x > max_x)
            max_x = v->x;
        if (v->y < min_y)
            min_y = v->y;
        if (v->y > max_y)
            max_y = v->y;
    }
    _center = Vec2(min_x + 0.5 * (max_x - min_x), min_y + 0.5 * (max_y - min_y));
    _size = Vec2(max_x - min_x, max_y - min_y);
    _vertices = vertices;
    for (auto v = _vertices.begin(); v != _vertices.end(); v++)
        *v = *v - _center;
    {
        Vec2 v0v1 = _vertices[0] - _vertices[_vertices.size() - 1];
        _normals.push_back(Vec2::normalize(Vec2::cross(v0v1)));
        for (unsigned int i = 1; i < _vertices.size(); i++) {
            Vec2 v0v1 = _vertices[i] - _vertices[i - 1];
            _normals.push_back(Vec2::normalize(Vec2::cross(v0v1)));
        }
    }
}

void Convex::scale(double factor, Vec2 anchor) {
    for (auto v = _vertices.begin(); v != _vertices.end(); v++)
        *v = *v * factor;
    _size = _size * factor;
}

void Convex::translate(Vec2 d) { _center = _center + d; }

void Convex::move_to(Vec2 p) { _center = p; }

std::vector<Vec2> Convex::vertices() const {
    std::vector<Vec2> out = _vertices;
    for (auto v = out.begin(); v != out.end(); v++)
        *v = *v + _center;
    return out;
}

std::vector<Vec2> Convex::normals() const { return _normals; }

Vec2 Convex::size() { return _size; }

Vec2 Convex::bottom_left() {
    double min_x = std::numeric_limits<double>::infinity();
    double min_y = std::numeric_limits<double>::infinity();
    for (auto v = _vertices.begin(); v != _vertices.end(); v++) {
        if (v->x < min_x)
            min_x = v->x;
        if (v->y < min_y)
            min_y = v->y;
    }
    return Vec2(min_x, min_y) + _center;
}

Vec2 Convex::center() { return _center; }

std::ostream &Convex::print(std::ostream &os) const {
    std::vector<Vec2> _vertices = vertices();
    return os << "Convex[" << _vertices << "]";
}

std::pair<double, double> project(Convex *a, Vec2 axis) {
    double min = std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();
    for (Vec2 v : a->vertices()) {
        double d = Vec2::dot(v, axis);
        if (d < min)
            min = d;
        if (d > max)
            max = d;
    }
    return std::make_pair(min, max);
}

std::pair<double, double> project(Circle *a, Vec2 axis) {
    double min = std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();
    Vec2 radius_on_axis = axis * a->radius;
    for (Vec2 v : {a->center() - radius_on_axis, a->center() + radius_on_axis}) {
        double d = Vec2::dot(v, axis);
        if (d < min)
            min = d;
        if (d > max)
            max = d;
    }
    return std::make_pair(min, max);
}

bool Convex::collide(Circle *a) {
    std::vector<Vec2> a_axes = normals();
    std::vector<Vec2> b_axes;
    // TODO this can be replaced with one axis, found as described in
    // https://www.metanetsoftware.com/technique/tutorialA.html#section3
    {
        std::vector<Vec2> vertices = this->vertices();
        for (auto v = vertices.rbegin(); v != vertices.rend(); v++)
            b_axes.push_back(Vec2::normalize(a->center() - *v));
    }
    for (std::vector<Vec2> axes : {a_axes, b_axes}) {
        for (Vec2 axis : axes) {
            std::pair<double, double> lra = project(this, axis);
            std::pair<double, double> lrb = project(a, axis);
            if (!(Inside(lra.first, lrb.first, lra.second) ||
                  Inside(lra.first, lrb.second, lra.second) ||
                  Inside(lrb.first, lra.first, lrb.second)))
                return false;
        }
    }
    return true;
}

bool Convex::collide(Convex *a) {
    std::vector<Vec2> a_axes = normals();
    std::vector<Vec2> b_axes = a->normals();
    for (std::vector<Vec2> axes : {a_axes, b_axes}) {
        for (Vec2 axis : axes) {
            std::pair<double, double> lra = project(this, axis);
            std::pair<double, double> lrb = project(a, axis);
            if (!(Inside(lra.first, lrb.first, lra.second) ||
                  Inside(lra.first, lrb.second, lra.second)))
                return false;
        }
    }
    return true;
}

Vec2 Convex::uncollide(Circle *s) {
    std::vector<Vec2> a_axes;
    // TODO this can be replaced with one axis, found as described in
    // https://www.metanetsoftware.com/technique/tutorialA.html#section3
    {
        std::vector<Vec2> vertices = this->vertices();
        for (auto v = vertices.rbegin(); v != vertices.rend(); v++)
            a_axes.push_back(Vec2::normalize(s->center() - *v));
    }
    std::vector<Vec2> b_axes = normals();
    Vec2 min_axis;
    double min_overlap = std::numeric_limits<double>::max();
    for (std::vector<Vec2> axes : {a_axes, b_axes}) {
        for (Vec2 axis : axes) {
            std::pair<double, double> lra = project(s, axis);
            std::pair<double, double> lrb = project(this, axis);
            double overlap = 0;
            if (Inside(lra.first, lrb.first, lra.second))
                overlap = lra.second - lrb.first;
            else if (Inside(lra.first, lrb.second, lra.second))
                overlap = lrb.second - lra.first;
            else if (Inside(lrb.first, lra.first, lrb.second))
                overlap = lra.second - lra.first;
            else
                return Vec2(0, 0);
            if (overlap < min_overlap) {
                min_overlap = overlap;
                min_axis = axis;
            }
        }
    }
    Vec2 vcenter = this->center() - s->center();
    double scalar_proj = Vec2::dot(vcenter, min_axis) / Vec2::dot(vcenter, vcenter);
    char direction = scalar_proj > 0 ? 1 : -1;
    min_axis = Vec2::times(min_axis, direction * min_overlap);
    return min_axis;
}

Vec2 Convex::uncollide(Convex *s) {
    std::vector<Vec2> a_axes = s->normals();
    std::vector<Vec2> b_axes = normals();
    Vec2 min_axis;
    double min_overlap = std::numeric_limits<double>::max();
    for (std::vector<Vec2> axes : {a_axes, b_axes}) {
        for (Vec2 axis : axes) {
            std::pair<double, double> lra = project(s, axis);
            std::pair<double, double> lrb = project(this, axis);
            double overlap = 0;
            if (Inside(lra.first, lrb.first, lra.second))
                overlap = lra.second - lrb.first;
            else if (Inside(lra.first, lrb.second, lra.second))
                overlap = lrb.second - lra.first;
            else
                return Vec2(0, 0);
            if (overlap < min_overlap) {
                min_overlap = overlap;
                min_axis = axis;
            }
        }
    }
    Vec2 vcenter = this->center() - s->center();
    double scalar_proj = Vec2::dot(vcenter, min_axis) / Vec2::dot(vcenter, vcenter);
    char direction = scalar_proj > 0 ? 1 : -1;
    min_axis = Vec2::times(min_axis, direction * min_overlap);
    return min_axis;
}

bool Convex::contains(Vec2 p) {
    std::vector<Vec2> axes = normals();
    for (Vec2 axis : axes) {
        std::pair<double, double> lra = project(this, axis);
        double d = Vec2::dot(p, axis);
        if (!Inside(lra.first, d, lra.second))
            return false;
    }
    return true;
}

//////

Rectangle::Rectangle(Vec2 center, Vec2 size)
    : Convex({-0.5 * size + center, Vec2(0.5 * size.x, -0.5 * size.y) + center, 0.5 * size + center,
              Vec2(-0.5 * size.x, 0.5 * size.y) + center}) {}

std::ostream &Rectangle::print(std::ostream &os) const {
    std::vector<Vec2> _vertices = vertices();
    return os << "Rectangle[" << _vertices[0] << ' ' << _vertices[2] - _vertices[0] << "]";
}

#undef Inside
