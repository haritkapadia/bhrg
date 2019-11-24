#ifndef MAP_HPP
#define MAP_HPP

#include <string>
#include <vector>
#include "bounds.hpp"
#include "region.hpp"

/**
 * A collection of solids and notable features of a world.
 */
class Map {
private:
  std::vector<PolygonRegion*> _solids;
public:
  /**
   * @return The solids in the world.
   */
  std::vector<PolygonRegion*>* solids();
  /**
   * Reads a map from a file created by write().
   *
   * @param filename   The name of the map file.
   */
  void read(std::string filename);
  /**
   * Writes a map file to be parsed by read().
   *
   * @param filename   The name of the map file.
   */
  void write(std::string filename);
};

#endif
