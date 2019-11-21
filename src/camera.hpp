
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SDL2/SDL.h>
#include "bounds.hpp"
#include "world.hpp"

class Camera : public Entity {
private:
  World* _world;
  int* SCREEN_WIDTH;
  int* SCREEN_HEIGHT;
public:
  Camera(World* _world,
         Position _position,
         RectangularBounds* _bounds,
         int* SCREEN_WIDTH,
         int* SCREEN_HEIGHT);
  void zoom(double scale);
  SDL_Point screen_transform(Position p);
  SDL_Rect screen_transform(Position p, Bounds* b);
};

#endif
