#ifndef PATH_HPP
#define PATH_HPP

#include "vec.hpp"
#include <memory>
#include <vector>

class PathComponent {
  public:
    // 0 <= t <= limit(), PathComponent is actually just a parametric equation
    virtual Vec2 solve(double t) = 0;
    virtual double limit() = 0;
};

class Segment : public PathComponent {
  private:
    Vec2 segment;

  public:
    Segment(Vec2 segment);
    virtual Vec2 solve(double t);
    virtual double limit();
};

class Path {
  private:
    Vec2 const start;
    std::vector<std::unique_ptr<PathComponent>> const components;

  public:
    Path(Vec2 start, std::vector<std::unique_ptr<PathComponent>> components);
    // 0 <= t <= 1, Path is actually also a parametric equation
    Vec2 solve(double t);
};

#endif
