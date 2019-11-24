#include "camera.hpp"

Camera::Camera(World* _world,
               Position _position,
               RectangularBounds* _bounds,
               int* SCREEN_WIDTH,
               int* SCREEN_HEIGHT) :
  Entity(new Region(_position, _bounds)),
  _world(_world),
  SCREEN_WIDTH(SCREEN_WIDTH),
  SCREEN_HEIGHT(SCREEN_HEIGHT) {}

void Camera::zoom(double scale) {
  region->bounds()->scale(1/scale);
}

// https://www.desmos.com/calculator/jvd4ny5zxa
SDL_Point Camera::screen_transform(Position p) {
  double x = *SCREEN_WIDTH * ((p.x - region->position.x) /
                              region->bounds()->size().x + 0.5);
  double y = *SCREEN_HEIGHT * (0.5 - (p.y - region->position.y) /
                               region->bounds()->size().y);
  return
    {
     (int)(x),
     (int)(y),
    };
}

SDL_Rect Camera::screen_transform(Region* _region) {
  Position p = _region->position;
  Bounds* b = _region->bounds();
  double x = *SCREEN_WIDTH * ((p.x - region->position.x) /
                              region->bounds()->size().x + 0.5);
  double y = *SCREEN_HEIGHT * (0.5 - (p.y - region->position.y) /
                               region->bounds()->size().y);
  double w = *SCREEN_WIDTH / region->bounds()->size().x * b->size().x;
  double h = *SCREEN_HEIGHT / region->bounds()->size().y * b->size().y;
  return
    {
     (int)(x - w/2),
     (int)(y - h/2),
     (int)(w),
     (int)(h)
    };
}
