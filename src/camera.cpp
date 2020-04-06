#include "camera.hpp"

Camera::Camera(Rectangle *region, int *SCREEN_WIDTH, int *SCREEN_HEIGHT)
    : Entity(EntityFactory().occupies({region})->create()), SCREEN_WIDTH(SCREEN_WIDTH),
      SCREEN_HEIGHT(SCREEN_HEIGHT) {}

void Camera::zoom(double scale) { occupies.region->scale(1 / scale, occupies.region->center()); }

// https://www.desmos.com/calculator/jvd4ny5zxa
SDL_Point Camera::screen_transform(Position p) {
    Vec2 center = occupies.region->center();
    Vec2 size = occupies.region->size();
    double x = *SCREEN_WIDTH * ((p.x - center.x) / size.x + 0.5);
    double y = *SCREEN_HEIGHT * (0.5 - (p.y - center.y) / size.y);
    return {(int)(x), (int)(y)};
}

SDL_Rect Camera::screen_transform(Region *_region) {
    Vec2 p = _region->center();
    Vec2 b = _region->size();
    Vec2 center = occupies.region->center();
    Vec2 size = occupies.region->size();
    double x = *SCREEN_WIDTH * ((p.x - center.x) / size.x + 0.5);
    double y = *SCREEN_HEIGHT * (0.5 - (p.y - center.y) / size.y);
    double w = *SCREEN_WIDTH / size.x * b.x;
    double h = *SCREEN_HEIGHT / size.y * b.y;
    return {(int)(x - w / 2), (int)(y - h / 2), (int)(w), (int)(h)};
}
