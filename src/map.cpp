#include "map.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>

#define NO_MAP_DEBUG

std::vector<PolygonRegion*>* Map::solids() {
  return &_solids;
}

// currently, maps can only have ConvexRegions
void Map::read(std::string filename) {
  unsigned int solid_count;
  std::ifstream fin;
  fin.open(filename, std::ios_base::binary);
  // reads the number of solids in the map
  fin.read((char*)&solid_count, sizeof(unsigned int));
#ifdef MAP_DEBUG
  std::cout << "Solids: " << solid_count << "\n\n";
#endif
  // allocates space for the solids
  _solids.reserve(solid_count);
  // reads and adds each solid
  for(unsigned int i = 0; i < solid_count; i++) {
    unsigned int vertex_count;
    Position center;
    // get vertex count and center
    fin.read((char*)&vertex_count, sizeof(unsigned int));
    fin.read((char*)&center.x, sizeof(double));
    fin.read((char*)&center.y, sizeof(double));
    Vec2 size;
    // get bounding box
    fin.read((char*)&size.x, sizeof(double));
    fin.read((char*)&size.y, sizeof(double));
    std::vector<Position> vertices;
#ifdef MAP_DEBUG
    std::cout << "Region " << i << " with " << vertex_count << ":\t"
              << center.x << ' ' << center.y << '\n';
#endif
    vertices.reserve(vertex_count);
    // get vertices, assumed to be in counterclockwise order
    for(unsigned int j = 0; j < vertex_count; j++) {
      Position vertex;
      fin.read((char*)&vertex.x, sizeof(double));
      fin.read((char*)&vertex.y, sizeof(double));
#ifdef MAP_DEBUG
      std::cout << "Vertex " << j << ":\t"
                << vertex.x << ' ' << vertex.y << '\n';
#endif
      vertices.push_back(vertex);
    }
#ifdef MAP_DEBUG
    std::cout << '\n';
#endif
    // add solid to map
    _solids.push_back(new PolygonRegion(center,
                                        new ConvexBounds(vertices, size)));
  }
  fin.close();
#ifdef MAP_DEBUG
  std::cout << "Done\n";
#endif
}

// TODO Change static_cast to support all PolygonBounds
void Map::write(std::string filename) {
  unsigned int solid_count = _solids.size();
  std::ofstream fout;
  fout.open(filename, std::ios_base::binary);
  // write the number of solids in the map
  fout.write((char*)&solid_count, sizeof(unsigned int));
  std::cout << "Solids: " << solid_count << "\n\n";
  
  // writes each solid
  for(unsigned int i = 0; i < solid_count; i++) {
    unsigned int vertex_count;
    vertex_count = static_cast<ConvexBounds*>(_solids[i]->bounds())->count();
    Position center = _solids[i]->position;
    // write vertex count and center
    fout.write((char*)&vertex_count, sizeof(unsigned int));
    fout.write((char*)&center.x, sizeof(double));
    fout.write((char*)&center.y, sizeof(double));
    Vec2 size = _solids[i]->bounds()->size();
    // write bounding box
    fout.write((char*)&size.x, sizeof(double));
    fout.write((char*)&size.y, sizeof(double));
    std::vector<Position> vertices = _solids[i]->bounds()->vertices();
    std::cout << "Region " << i << " with " << vertex_count << ":\t"
              << center.x << ' ' << center.y << '\n';

    // write vertices, assumed to be in counterclockwise order
    for(unsigned int j = 0; j < vertex_count; j++) {
      Position vertex = vertices[j];
      fout.write((char*)&vertex.x, sizeof(double));
      fout.write((char*)&vertex.y, sizeof(double));
      std::cout << "Vertex " << j << ":\t"
                << vertex.x + center.x << ' ' << vertex.y + center.y << '\n';

    }
    std::cout << '\n';
    

    
  }
  fout.close();
  
}
