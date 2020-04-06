#include "map.hpp"
#include "debug.hpp"
#include <algorithm>
#include <functional>
#include <iostream>

void Map::read(std::istream *fin) {
    unsigned int solid_count;
    // reads the number of solids in the map
    fin->read((char *)&solid_count, sizeof(unsigned int));
    debug::print("Solids:", solid_count);
    // allocates space for the solids
    solids.reserve(solid_count);
    // reads and adds each solid
    for (unsigned int i = 0; i < solid_count; i++) {
        unsigned int region_type;
        fin->read((char *)&region_type, sizeof(unsigned int));
        switch (region_type) {
        case 0: {
            Vec2 center;
            Vec2 size;
            fin->read((char *)&center.x, sizeof(double));
            fin->read((char *)&center.y, sizeof(double));
            fin->read((char *)&size.x, sizeof(double));
            fin->read((char *)&size.y, sizeof(double));
            debug::print("Rectangle at", center, "with dimensions", size);
            solids.push_back(new Rectangle(center, size));
            break;
        }
        case 1: {
            unsigned int vertex_count;
            fin->read((char *)&vertex_count, sizeof(unsigned int));
            std::vector<Vec2> vertices;
            debug::print("Convex", i, "with", vertex_count, "vertices");
            vertices.reserve(vertex_count);
            for (unsigned int j = 0; j < vertex_count; j++) {
                Vec2 vertex;
                fin->read((char *)&vertex.x, sizeof(double));
                fin->read((char *)&vertex.y, sizeof(double));
                vertices.push_back(vertex);
            }
            debug::print(vertices);
            solids.push_back(new Convex(vertices));
            break;
        }
        case 2: {
            Vec2 center;
            double radius;
            fin->read((char *)&center.x, sizeof(double));
            fin->read((char *)&center.y, sizeof(double));
            fin->read((char *)&radius, sizeof(double));
            debug::print("Circle at", center, "with radius", radius);
            solids.push_back(new Circle(center, radius));
            break;
        }
        default:
            break;
        }
    }
    debug::print("Done reading map.");
}

void Map::write(std::ostream *fout) {
    unsigned int solid_count = solids.size();
    // write the number of solids in the map
    fout->write((char *)&solid_count, sizeof(unsigned int));
    debug::print("Solids:", solid_count);

    // writes each solid
    for (unsigned int i = 0; i < solid_count; i++) {
        if (auto s = dynamic_cast<Rectangle *>(solids[i])) {
            unsigned int region_type = 0;
            fout->write((char *)&region_type, sizeof(unsigned int));
            Vec2 center = s->center();
            Vec2 size = s->size();
            debug::print("Rectangle at", center, "with dimensions", size);
            fout->write((char *)&center.x, sizeof(double));
            fout->write((char *)&center.y, sizeof(double));
            fout->write((char *)&size.x, sizeof(double));
            fout->write((char *)&size.y, sizeof(double));
        } else if (auto s = dynamic_cast<Convex *>(solids[i])) {
            unsigned int region_type = 1;
            fout->write((char *)&region_type, sizeof(unsigned int));
            std::vector<Vec2> vertices = s->vertices();
            unsigned int vertex_count = vertices.size();
            fout->write((char *)&vertex_count, sizeof(unsigned int));
            debug::print("Convex", i, "with", vertex_count, "vertices");
            for (unsigned int j = 0; j < vertex_count; j++) {
                Vec2 vertex = vertices[j];
                fout->write((char *)&vertex.x, sizeof(double));
                fout->write((char *)&vertex.y, sizeof(double));
            }
        } else if (auto s = dynamic_cast<Circle *>(solids[i])) {
            unsigned int region_type = 2;
            fout->write((char *)&region_type, sizeof(unsigned int));
            Vec2 center = s->center();
            double radius = s->radius;
            debug::print("Circle at", center, "with radius", radius);
            fout->write((char *)&center.x, sizeof(double));
            fout->write((char *)&center.y, sizeof(double));
            fout->write((char *)&radius, sizeof(double));
        }
    }
    debug::print("Done writing map.");
}
