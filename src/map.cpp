#include "map.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>

std::vector<PolygonRegion*
            >* Map::solids() {
  return &_solids;
}

void Map::read(std::string filename) {
  unsigned int solid_count = 11;
  std::ifstream fin;
  fin.open(filename, std::ios_base::binary);
  fin.read((char*)&solid_count, sizeof(unsigned int));
  std::cout << "Solids: " << solid_count << "\n\n";
  _solids.reserve(solid_count);
  for(unsigned int i = 0; i < solid_count; i++) {
    unsigned int vertex_count;
    Position center;
    fin.read((char*)&vertex_count, sizeof(unsigned int));
    fin.read((char*)&center.x, sizeof(double));
    fin.read((char*)&center.y, sizeof(double));
    Vec2 size;
    fin.read((char*)&size.x, sizeof(double));
    fin.read((char*)&size.y, sizeof(double));
    std::vector<Position> vertices;
    std::cout << "Region " << i << " with " << vertex_count << ":\t"
              << center.x << ' ' << center.y << '\n';
    vertices.reserve(vertex_count);
    for(unsigned int j = 0; j < vertex_count; j++) {
      Position vertex;
      fin.read((char*)&vertex.x, sizeof(double));
      fin.read((char*)&vertex.y, sizeof(double));
      std::cout << "Vertex " << j << ":\t"
                << vertex.x << ' ' << vertex.y << '\n';
      vertices.push_back(vertex);
    }
    std::cout << '\n';
    _solids.push_back(new PolygonRegion(center, new ConvexBounds(vertices, size)));
  }
  fin.close();
  std::cout << "Done\n";
}

// TODO Change static_cast to support all PolygonBounds
void Map::write(std::string filename) {
  unsigned int solid_count = _solids.size();
  std::ofstream fout;
  fout.open(filename, std::ios_base::binary);
  fout.write((char*)&solid_count, sizeof(unsigned int));
  std::cout << "Solids: " << solid_count << "\n\n";
  
  for(unsigned int i = 0; i < solid_count; i++) {
    unsigned int vertex_count = static_cast<ConvexBounds*>(_solids[i]->bounds())->count();
    Position center = _solids[i]->position;
    fout.write((char*)&vertex_count, sizeof(unsigned int));
    fout.write((char*)&center.x, sizeof(double));
    fout.write((char*)&center.y, sizeof(double));
    std::vector<Position> vertices = _solids[i]->bounds()->vertices();
    Vec2 size = _solids[i]->bounds()->size();
    fout.write((char*)&size.x, sizeof(double));
    fout.write((char*)&size.y, sizeof(double));
    std::cout << "Region " << i << " with " << vertex_count << ":\t"
              << center.x << ' ' << center.y << '\n';

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
