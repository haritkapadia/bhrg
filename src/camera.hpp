
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SDL2/SDL.h>
#include "bounds.hpp"
#include "world.hpp"

/**
 * A Camera reference that exists in the World to provide information about
 * the bounds available for the player to see.
 */
class Camera : public Entity {
private:
  World* _world;
  int* SCREEN_WIDTH;
  int* SCREEN_HEIGHT;
public:
  /**
   * @param _world          The world that the Camera is in. Note that the
   *                        camera isn't spawned in.
   * @param _position       The position of the Camera in the world.
   * @param _bounds         The bounds of the Camera viewing window.
   * @param screen_width    The width of the screen.
   * @param screen_height   The height of the screen.
   */
  Camera(World* _world,
         Position _position,
         RectangularBounds* _bounds,
         int* screen_width,
         int* screen_height);
  /**
   * Changes the size of the camera bounds by a divisive factor.
   *
   * @param scale   The zoom factor.
   */
  void zoom(double scale);
  /**
   * Transforms a world coordinate to a screen coordinate.
   *
   * @param position   The world coordinate.
   */
  SDL_Point screen_transform(Position p);
  /**
   * Transforms a region in the world to a bounding box of the region on the
   * screen.
   *
   * @param _region   The region in the world.
   */
  SDL_Rect screen_transform(Region* _region);
};

#endif
