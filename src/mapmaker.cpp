#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <limits>
#include <vector>
#include <SDL2/SDL.h>


#include "bounds.hpp"
#include "camera.hpp"
#include "map.hpp"


template <typename T>
T Constrain(T val, T low, T high) {
  if(val < low)
    return low;
  if(val > high)
    return high;
  return val;
}


void fill_circle(SDL_Renderer* renderer, int cx, int cy, int radius) {
  for(int i = -radius; i <= radius; i++) {
    int height = sqrt(radius * radius - i * i);
    SDL_RenderDrawLine(renderer, cx + i, cy - height, cx + i, cy + height);
  }
}

int main(int argc, char* argv[]) {
  SDL_Window* window;
  SDL_Renderer* renderer;
  bool running;

  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Could not initialise SDL. SDL_Error: " << SDL_GetError() << '\n';
  }
  int SCREEN_WIDTH = 500, SCREEN_HEIGHT = 500;
  window = SDL_CreateWindow("Mapmaker: Bullet Hell Rhythm Game",
                            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
  if(window == NULL) {
    std::cerr << "Could not create window. ";
    std::cerr << "SDL_Error: " << SDL_GetError() << '\n';
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  running = true;
  unsigned long long prev_ticks, curr_ticks;
  prev_ticks = SDL_GetTicks();
  World world;
  Player player = Player(new PolygonRegion({0, 0}, new RectangularBounds({1, 1})));
  world.spawn(&player);
  Camera camera = Camera(&world,
                         player.region->position,
                         new RectangularBounds({10, 10}),
                         &SCREEN_WIDTH,
                         &SCREEN_HEIGHT);
  std::vector<Position> vertices;
  int vertices_size = 0;
  Map map;
  // map.read("dabb.map");
  std::vector<PolygonRegion*>* solids = map.solids();
  bool ctrl_press = false;
  
  while(running) {
    curr_ticks = SDL_GetTicks();
    SDL_Event e;
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    // https://www.desmos.com/calculator/jvd4ny5zxa
    double target_x = (camera.region->bounds()->size().x / SCREEN_WIDTH) *
      (mx + camera.region->position.x * SCREEN_WIDTH / camera.region->bounds()->size().x - SCREEN_WIDTH / 2);
    double target_y = (camera.region->bounds()->size().y / SCREEN_HEIGHT) *
      (-my + camera.region->position.y * SCREEN_HEIGHT / camera.region->bounds()->size().y + SCREEN_HEIGHT / 2);
    while(SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_MOUSEBUTTONDOWN) {
        double new_x = target_x;
        double new_y = target_y;
        if(ctrl_press) {
          new_x = std::round(target_x);
          new_y = std::round(target_y);
        }
        if((int)vertices.size() <= vertices_size)
          vertices.push_back({new_x, new_y});
        else
          vertices[vertices_size] = {new_x, new_y};
        vertices_size += 1;
      } else if(e.type == SDL_MOUSEWHEEL) {
        if(e.wheel.y < 0) {
          camera.zoom(1.25);
        } else if(e.wheel.y > 0) {
          camera.zoom(0.8);
        }
      } else if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
        case SDLK_LCTRL:
          // falls through
        case SDLK_RCTRL:
          ctrl_press = true;
          break;
        case SDLK_ESCAPE:
          vertices.clear();
          vertices_size = 0;
          break;
        case SDLK_SPACE: {
          std::vector<Position> vbounds(vertices.begin(),
                                        vertices.begin() + vertices_size);
          double l = std::numeric_limits<double>::max();
          double r = -std::numeric_limits<double>::max();
          double d = std::numeric_limits<double>::max();
          double u = -std::numeric_limits<double>::max();
          for(Position p : vbounds) {
            std::cout << l << ' ' << r << ' ' << u << ' ' << d << '\n';
            if(p.x < l) l = p.x;
            if(p.x > r) r = p.x;
            if(p.y < d) d = p.y;
            if(p.y > u) u = p.y;
          }
          std::cout << "Done: " << l << ' ' << r << ' ' << u << ' ' << d << "\n\n";
          const Position center = {(l + r) / 2, (u + d) / 2};
          for(auto it = vbounds.begin(); it != vbounds.end(); it++) {
            it->x -= center.x;
            it->y -= center.y;
          }
          solids->push_back(new PolygonRegion(center, new ConvexBounds(vbounds, {r - l, u - d})));
          vertices.clear();
          vertices_size = 0;
          break;
        }
        case SDLK_LEFT:
          vertices_size -= 1;
          vertices_size = Constrain(vertices_size, 0, (int)vertices.size());
          break;
        case SDLK_RIGHT:
          vertices_size += 1;
          vertices_size = Constrain(vertices_size, 0, (int)vertices.size());
          break;
        case SDLK_w:
          player.velocity.y += 3;
          break;
        case SDLK_s:
          player.velocity.y += -3;
          break;
        case SDLK_a:
          player.velocity.x += -3;
          break;
        case SDLK_d:
          player.velocity.y += 3;
          break;
        default:
          break;
        }
      } else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
        case SDLK_LCTRL:
          // falls through
        case SDLK_RCTRL:
          ctrl_press = false;
          break;
        case SDLK_w:
          player.velocity.y -= 3;
          break;
        case SDLK_s:
          player.velocity.y -= -3;
          break;
        case SDLK_a:
          player.velocity.x -= -3;
          break;
        case SDLK_d:
          player.velocity.y -= 3;
          break;
        default:
          break;
        }
      } else if(e.type == SDL_QUIT) {
        running = false;
      } else if(e.type == SDL_WINDOWEVENT) {
        SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
      }
    }

    player.move((curr_ticks - prev_ticks) / 1000.0);
    camera.region->position = player.region->position;

    // drawing grid
    {
      SDL_SetRenderDrawColor(renderer, 63, 63, 63, 255);
      int j = 0;
      if(camera.region->position.x - camera.region->bounds()->size().x / 2 > 0)
        j = std::ceil(camera.region->position.x - camera.region->bounds()->size().x / 2);
      else
        j = 1 + std::floor(camera.region->position.x - camera.region->bounds()->size().x / 2);
      for(int i = 0; i < camera.region->bounds()->size().x; i++) {
        int a = (int)((SCREEN_WIDTH / camera.region->bounds()->size().x) * (j + i - camera.region->position.x) + SCREEN_WIDTH / 2.0);
        SDL_RenderDrawLine(renderer, a, 0, a, SCREEN_HEIGHT);
      }
      if(camera.region->position.y - camera.region->bounds()->size().y / 2 > 0)
        j = std::ceil(camera.region->position.y - camera.region->bounds()->size().y / 2);
      else
        j = 1 + std::floor(camera.region->position.y - camera.region->bounds()->size().y / 2);
      for(int i = 0; i < camera.region->bounds()->size().y; i++) {
        int a = (int)(SCREEN_HEIGHT / 2.0 - (SCREEN_HEIGHT / camera.region->bounds()->size().y) * (j + i - camera.region->position.y));
        SDL_RenderDrawLine(renderer, 0, a, SCREEN_WIDTH, a);
      }
    }
    // drawing solids
    for(unsigned int j = 0; j < solids->size(); j++) {
      uint8_t val = (128 + j * 25) % 255;
      SDL_SetRenderDrawColor(renderer, val, val / 2, (val * 2) % 255, 255);
      std::vector<Position> vertices = (*solids)[j]->vertices();
      SDL_Point points[vertices.size() + 1];
      for(unsigned int i = 0; i < vertices.size(); i++) {
        points[i] = camera.screen_transform(vertices[i]);
        fill_circle(renderer, points[i].x, points[i].y, 5);
      }
      points[vertices.size()] = points[0];
      SDL_RenderDrawLines(renderer, points, vertices.size() + 1);
      SDL_Rect rect = camera.screen_transform((*map.solids())[j]->position,
                                              (*map.solids())[j]->bounds());
      SDL_RenderDrawRect(renderer, &rect);
    }
    // drawing current construction
    {
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_Point points[vertices_size + 1];
      for(int i = 0; i < vertices_size; i++) {
        points[i] = camera.screen_transform(vertices[i]);
        fill_circle(renderer, points[i].x, points[i].y, 7);
      }
      points[vertices_size] = {mx, my};
      SDL_RenderDrawLines(renderer, points, vertices_size + 1);
    }
    SDL_RenderPresent(renderer);
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    prev_ticks = curr_ticks;
    SDL_Delay(16);
  }

  map.write("dabb.map");

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
