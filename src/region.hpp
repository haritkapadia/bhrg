#ifndef REGION_HPP
#define REGION_HPP

#include "vec.hpp"
#include <ostream>
#include <string>
#include <vector>

class Region {
  public:
    virtual void scale(double factor, Vec2 anchor) = 0;
    virtual void translate(Vec2 d) = 0;
    virtual void move_to(Vec2 p) = 0;
    virtual Vec2 size() = 0;
    virtual Vec2 bottom_left() = 0;
    virtual Vec2 center() = 0;
    virtual std::ostream &print(std::ostream &os) const = 0;
    friend std::ostream &operator<<(std::ostream &os, Region const &r);
    virtual bool contains(Vec2 p) = 0;
    // r = region, s = solid. r moves
    static Vec2 uncollide(Region *r, Region *s);
    static bool might_collide(Region *a, Region *b);
    static bool collide(Region *a, Region *b);
};

class Circle : public Region {
  private:
    Vec2 _center;

  public:
    double radius;
    Circle(Vec2 center, double radius);
    virtual void scale(double factor, Vec2 anchor);
    virtual void translate(Vec2 d);
    virtual void move_to(Vec2 p);
    virtual Vec2 size();
    virtual Vec2 bottom_left();
    virtual Vec2 center();
    virtual std::ostream &print(std::ostream &os) const;
    virtual bool contains(Vec2 p);
    bool collide(Circle *a);
    Vec2 uncollide(Circle *s);
};

class Convex : public Region {
  private:
    std::vector<Vec2> _vertices;
    std::vector<Vec2> _normals;
    Vec2 _center;
    Vec2 _size;

  public:
    Convex(std::vector<Vec2> vertices);
    virtual void scale(double factor, Vec2 anchor);
    virtual void translate(Vec2 d);
    virtual void move_to(Vec2 p);
    std::vector<Vec2> vertices() const;
    std::vector<Vec2> normals() const;
    virtual Vec2 size();
    virtual Vec2 bottom_left();
    virtual Vec2 center();
    virtual std::ostream &print(std::ostream &os) const;
    virtual bool contains(Vec2 p);
    bool collide(Circle *a);
    bool collide(Convex *a);
    Vec2 uncollide(Circle *s);
    Vec2 uncollide(Convex *s);
};

class Rectangle : public Convex {
  public:
    Rectangle(Vec2 center, Vec2 size);
    virtual std::ostream &print(std::ostream &os) const;
};

#endif
