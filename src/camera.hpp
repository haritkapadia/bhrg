#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "entity.hpp"
#include "region.hpp"
#include <SDL2/SDL.h>

/**
 * A Camera reference that exists in the World to provide information about
 * the bounds available for the player to see.
 */
class Camera : public Entity {
  private:
    int *SCREEN_WIDTH;
    int *SCREEN_HEIGHT;

  public:
    /**
     // * @param _world          The world that the Camera is in. Note that the
     // *                        camera isn't spawned in.
     * @param region          The region of the Camera viewing window.
     * @param screen_width    The width of the screen.
     * @param screen_height   The height of the screen.
     */
    Camera(Rectangle *region, int *screen_width, int *screen_height);
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
    SDL_Point screen_transform(Vec2 p);
    /**
     * Transforms a region in the world to a bounding box of the region on the
     * screen.
     *
     * @param _region   The region in the world.
     */
    SDL_Rect screen_transform(Region *_region);
};

#endif
