#ifndef MAP_HPP
#define MAP_HPP

#include <string>
#include <vector>
#include "bounds.hpp"
#include "region.hpp"

class Map {
private:
  std::vector<PolygonRegion*> _solids;
public:
  std::vector<PolygonRegion*>* solids();
  void read(std::string filename);
  void write(std::string filename);
};

#endif
