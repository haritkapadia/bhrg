#ifndef MAP_HPP
#define MAP_HPP

#include "region.hpp"
#include <istream>
#include <ostream>
#include <vector>

/**
 * A collection of solids and notable features of a world.
 */
class Map {
  public:
    std::vector<Region *> solids;
    /**
     * Reads a map from a file created by write().
     *
     * @param fin   The input stream containing the map.
     */
    void read(std::istream *fin);
    /**
     * Writes a map file to be parsed by read().
     *
     * @param fout   The output stream to which to write the map
     */
    void write(std::ostream *filename);
};

#endif
