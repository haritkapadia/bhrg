#include "map.hpp"
#include "debug.hpp"
#include <algorithm>
#include <functional>
#include <iostream>

std::vector<PolygonRegion *> *Map::solids() { return &_solids; }

// currently, maps can only have ConvexRegions
void Map::read(std::istream *fin) {
    unsigned int solid_count;
    // reads the number of solids in the map
    fin->read((char *)&solid_count, sizeof(unsigned int));
    debug::print("Solids:", solid_count);
    // allocates space for the solids
    _solids.reserve(solid_count);
    // reads and adds each solid
    for (unsigned int i = 0; i < solid_count; i++) {
        unsigned int vertex_count;
        Position center;
        // get vertex count and center
        fin->read((char *)&vertex_count, sizeof(unsigned int));
        fin->read((char *)&center.x, sizeof(double));
        fin->read((char *)&center.y, sizeof(double));
        Vec2 size;
        // get bounding box
        fin->read((char *)&size.x, sizeof(double));
        fin->read((char *)&size.y, sizeof(double));
        std::vector<Position> vertices;
        debug::print("Region", i, "with", vertex_count, "vertices around", center);
        vertices.reserve(vertex_count);
        // get vertices, assumed to be in counterclockwise order
        for (unsigned int j = 0; j < vertex_count; j++) {
            Position vertex;
            fin->read((char *)&vertex.x, sizeof(double));
            fin->read((char *)&vertex.y, sizeof(double));
            debug::print("Vertex", j, "\t", vertex);
            vertices.push_back(vertex);
        }
        // add solid to map
        _solids.push_back(new PolygonRegion(center, new ConvexBounds(vertices, size)));
    }
    debug::print("Done reading map.");
}

// TODO Change static_cast to support all PolygonBounds
void Map::write(std::ostream *fout) {
    unsigned int solid_count = _solids.size();
    // write the number of solids in the map
    fout->write((char *)&solid_count, sizeof(unsigned int));
    debug::print("Solids:", solid_count);

    // writes each solid
    for (unsigned int i = 0; i < solid_count; i++) {
        unsigned int vertex_count;
        vertex_count = static_cast<ConvexBounds *>(_solids[i]->bounds())->count();
        Position center = _solids[i]->position;
        // write vertex count and center
        fout->write((char *)&vertex_count, sizeof(unsigned int));
        fout->write((char *)&center.x, sizeof(double));
        fout->write((char *)&center.y, sizeof(double));
        Vec2 size = _solids[i]->bounds()->size();
        // write bounding box
        fout->write((char *)&size.x, sizeof(double));
        fout->write((char *)&size.y, sizeof(double));
        std::vector<Position> vertices = _solids[i]->bounds()->vertices();
        debug::print("Region", i, "with", vertex_count, "vertices around", center);

        // write vertices, assumed to be in counterclockwise order
        for (unsigned int j = 0; j < vertex_count; j++) {
            Position vertex = vertices[j];
            fout->write((char *)&vertex.x, sizeof(double));
            fout->write((char *)&vertex.y, sizeof(double));
            debug::print("Vertex", j, "\t", vertex);
        }
    }
    debug::print("Done writing map.");
}
