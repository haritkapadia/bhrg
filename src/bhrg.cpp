#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>
#include <algorithm>
#include <limits>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <essentia/algorithmfactory.h>
#include <essentia/pool.h>

#include "bounds.hpp"
#include "entity.hpp"
#include "world.hpp"
#include "spell.hpp"
#include "effect.hpp"
#include "camera.hpp"
#include "map.hpp"
#include "vec.hpp"

#define SAMPLE_RATE 44100
using namespace essentia;



/**
 * Places a filled circle onto the renderer.
 *
 * @param renderer   The renderer.
 * @param cx         The screen x-coordinate of the center of the circle.
 * @param cy         The screen y-coordinate of the center of the circle.
 * @param radius     The radius of the circle in pixels
 */
void fill_circle(SDL_Renderer* renderer, int cx, int cy, int radius) {
  for(int i = -radius; i <= radius; i++) {
    int height = sqrt(radius * radius - i * i);
    SDL_RenderDrawLine(renderer, cx + i, cy - height, cx + i, cy + height);
  }
}

/**
 * Draws a grid onto the screen with lattice points being 1 world unit apart.
 *
 * @param renderer   The renderer.
 * @param camera     The world camera.
 * @param width      The width of the screen.
 * @param height     The height of the screen.
 */
void draw_grid(SDL_Renderer* renderer, Camera* camera, int width, int height) {
  // j contains the leftmost integer grid position
  int j = 0;
  double gp = 0;
  Vec2 camera_position = camera->region->position;
  Vec2 camera_size = camera->region->bounds()->size();
  // j is calculated for width here
  if(camera->region->position.x - camera_size.x / 2 > 0)
    j = std::ceil(camera_position.x - camera_size.x / 2);
  else
    j = 1 + std::floor(camera_position.x - camera_size.x / 2);
  // draws the vertical lines for the grid from the left of the screen
  // this happens by transforming the left bounds of the camera position
  // to screen position and then moving it to the centre
  gp = (width / camera_size.x) * (j - camera_position.x) + width / 2;
  for(int i = 0; i < camera_size.x; i++) {
    SDL_RenderDrawLine(renderer, (int)gp, 0, (int)gp, height);
    // note that since the grid width is consistent,
    // the position can increase linearly
    gp += width / camera_size.x;
  }
  // j is calculated for height here
  if(camera_position.y - camera_size.y / 2 > 0)
    j = std::ceil(camera_position.y - camera_size.y / 2);
  else
    j = 1 + std::floor(camera_position.y - camera_size.y / 2);
  // draws the horizontal lines for the grid from from the top of the scren
  // gp is calculated similarly as for the vertical lines
  gp = height / 2 - (height / camera_size.y) * (j - camera_position.y);
  for(int i = 0; i < camera_size.y; i++) {
    SDL_RenderDrawLine(renderer, 0, gp, width, gp);
    gp += height / camera_size.y;
  }
}

// undocumented function
// This function will likely be rewritten and moved to a different file
// Analyses song using essentia, specifically finding beat markings and bpm
void analyse_song(std::string filename, Real* bpm, std::vector<Real>* ticks) {
  standard::AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
  standard::Algorithm* audio = factory.create("MonoLoader",
                                              "filename", filename,
                                              "sampleRate", SAMPLE_RATE);
  standard::Algorithm* rhythm_extractor = factory.create("RhythmExtractor2013");
  std::vector<Real> audio_buffer;
  essentia::Real confidence;
  std::vector<Real> estimates;
  std::vector<Real> bpm_intervals;
  audio->output("audio").set(audio_buffer); // acquire audio stream from file
  audio->compute();
  rhythm_extractor->input("signal").set(audio_buffer); // set input stream
  rhythm_extractor->output("bpm").set(*bpm);           // set output streams
  rhythm_extractor->output("ticks").set(*ticks);
  rhythm_extractor->output("confidence").set(confidence);
  rhythm_extractor->output("estimates").set(estimates);
  rhythm_extractor->output("bpmIntervals").set(bpm_intervals);
  rhythm_extractor->compute();  // get values and get out
  delete audio;
  delete rhythm_extractor;
}

// argc, argv not used
int main(int argc, char* argv[]) {
  SDL_Window* window;
  SDL_Renderer* renderer;
  Mix_Music* music;             // Music playing for SDL
  bool running = false;
  int SCREEN_WIDTH = 500, SCREEN_HEIGHT = 500;



  // SOUND STUFF
  std::string music_file = "./music/mt. fujitive - trees.mp3";
  essentia::init();
  essentia::Pool pool;
  essentia::Real bpm = 80;
  std::vector<essentia::Real> ticks;
  {
    unsigned long long start = SDL_GetTicks();
    // Song is currently ignored because analysis takes 10 seconds every run
    // analyse_song(music_file, &bpm, &ticks);
    unsigned long long end = SDL_GetTicks();
    std::cout << "Song analysis took " << (end - start) / 1000. << " seconds.\n";
  }


  // initialising required SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Could not initialise SDL. ";
    std::cerr << "SDL_Error: " << SDL_GetError() << '\n';
  }
  window = SDL_CreateWindow("Bullet Hell Rhythm Game",
                            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
  if(window == NULL) {
    std::cerr << "Could not create window. ";
    std::cerr << "SDL_Error: " << SDL_GetError() << '\n';
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "Could not create window. ";
    std::cerr << "Mix_Error: " << Mix_GetError() << '\n';
  }
  music = Mix_LoadMUS(music_file.c_str());
  if(music == NULL) {
    std::cerr << "Could not load music. ";
    std::cerr << "Mix_Error: " << Mix_GetError() << '\n';
  }

  running = true;
  Mix_PlayMusic(music, 0);
  unsigned long long start = SDL_GetTicks();
  unsigned long long prev_ticks, curr_ticks;
  // These values are used to change visuals according to the analysed song
  unsigned int tempo_color = 0, beat_color = 0;
  unsigned long long ibeat = 0;
  double spb = 0;
  // The playing region is loaded here
  World world;
  Map* map = world.map();
  map->read("dabb.map");
  Player player = Player(new PolygonRegion({0, 0},
                                           new RectangularBounds({1, 1})),
                         100);
  world.spawn(&player);


  Spell teleport = Spell(&world, Spell::SELF, {new Teleport()}, 1);
  Spell hurt = Spell(&world, Spell::SELF, {new Damage(10)}, 1);
  Spell fire = Spell(&world, Spell::PROJECTILE, {new Damage(10)}, 1);

  prev_ticks = SDL_GetTicks();
  Camera camera = Camera(&world,
                         Vec2(0, 0),
                         new RectangularBounds(Vec2(10, 10)),
                         &SCREEN_WIDTH,
                         &SCREEN_HEIGHT);
  bool show_grid = false;
  // Main event loop
  while(running) {
    curr_ticks = SDL_GetTicks();
    SDL_Event e;
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    // screen mouse coordinates transformed to world coordinates
    double target_x = (camera.region->bounds()->size().x / SCREEN_WIDTH) *
      (mx + camera.region->position.x * SCREEN_WIDTH /
       camera.region->bounds()->size().x - SCREEN_WIDTH / 2);
    double target_y = (camera.region->bounds()->size().y / SCREEN_HEIGHT) *
      (-my + camera.region->position.y * SCREEN_HEIGHT /
       camera.region->bounds()->size().y + SCREEN_HEIGHT / 2);
    while(SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_MOUSEWHEEL) {
        if(e.wheel.y < 0) {
          camera.zoom(0.8);
        } else if(e.wheel.y > 0) {
          camera.zoom(1.25);
        }
      }
      if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
          // quit game
        case SDLK_ESCAPE:
          running = false;
          break;
          // increases velocity upon WASD press
        case SDLK_w:
          player.velocity.y += 10;
          break;
        case SDLK_s:
          player.velocity.y += -10;
          break;
        case SDLK_a:
          player.velocity.x += -10;
          break;
        case SDLK_d:
          player.velocity.x += 10;
          break;
          // adjust camera position using arrow keys
        case SDLK_UP:
          camera.region->position.y += camera.region->bounds()->size().y / 10;
          break;
        case SDLK_DOWN:
          camera.region->position.y += -camera.region->bounds()->size().y / 10;
          break;
        case SDLK_LEFT:
          camera.region->position.x += -camera.region->bounds()->size().x / 10;
          break;
        case SDLK_RIGHT:
          camera.region->position.x += camera.region->bounds()->size().x / 10;
          break;
          // toggles grid
        case SDLK_g:
          show_grid = !show_grid;
          break;
          // fires a bullet starting at the player
        case SDLK_1:
          teleport.use(&player, {target_x, target_y});
          break;
        case SDLK_2:
          hurt.use(&player, Vec2::zero);
          break;
        case SDLK_3:
          fire.use(&player, {target_x, target_y});
          break;
        case SDLK_SPACE:
          {
            Bullet* b = new Bullet(new Region(player.region->position,
                                              new CircularBounds(0.3)));
            // bullet speed is dependent on initial mouse proximity to player
            b->velocity = Vec2(target_x, target_y) - b->region->position;
            world.spawn(b);
          }
          break;
        default:
          break;
        }
      } else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
          // decreases velocity upon WASD release
        case SDLK_w:
          player.velocity.y -= 10;
          break;
        case SDLK_s:
          player.velocity.y -= -10;
          break;
        case SDLK_a:
          player.velocity.x -= -10;
          break;
        case SDLK_d:
          player.velocity.x -= 10;
        default:
          break;
        }
      } else if(e.type == SDL_QUIT) {
        running = false;
      } else if(e.type == SDL_WINDOWEVENT) {
        // ensures the screen size is updated
        SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
      }
    }

    // change colour of beat box based on bpm
    unsigned long long now = SDL_GetTicks() - start;
    if(now >= spb * 1000) {
      tempo_color = (tempo_color + 15) % 255;
      spb += 60.0 / bpm;
    }
    // change colour of beat box based on beat markers
    if(ticks.size() > 0) {
      if(now >= ticks[ibeat] * 1000) {
        beat_color = (beat_color + 30) % 255;
        ibeat += 1;
        if(ibeat >= ticks.size())
          break;
      }
    }

    
    // moves all entities and projectiles in the world
    {
      double duration = (curr_ticks - prev_ticks) / 1000.0;
      for(MovingEntity* e : *world.moving_entities())
        e->move(duration);
      world.move_projectiles(duration);
    }
    
    // collision detection: player with solids
    bool collided = false;
    for(PolygonRegion* s : *map->solids()) {
      if(s->might_collide(player.region)) {
        Vec2 translation = player.region->min_translate(s);
        if(translation != Vec2::zero) {
          collided = true;
          player.region->position = player.region->position + translation;
        }
      }
    }


    
    // clear screen, beginning the drawing cycle
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // indicate if player collided with a solid
    if(collided) {
      SDL_Rect rect = {0, 40, 20, 20};
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      SDL_RenderFillRect(renderer, &rect);
    }

    SDL_Rect rect;
    // draw bpm indicator
    SDL_SetRenderDrawColor(renderer, tempo_color, 100, tempo_color, 255);
    rect = {0, 0, 20, 20};
    SDL_RenderFillRect(renderer, &rect);
    // draw beat marker indicator
    SDL_SetRenderDrawColor(renderer, beat_color, 255, beat_color, 255);
    rect = {0, 20, 20, 20};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // draw grid
    if(show_grid) {
      SDL_SetRenderDrawColor(renderer, 63, 63, 63, 255);
      draw_grid(renderer, &camera, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    
    // draw solids
    for(unsigned int j = 0; j < map->solids()->size(); j++) {
      unsigned int val = (128 + j * 25) % 255; // each shape has a new colour
      SDL_SetRenderDrawColor(renderer, val, val / 2, (val * 2) % 255, 255);
      // recording vertices, points, and axes
      // to draw   vertices, sides,  and normals
      std::vector<Position> vertices = (*map->solids())[j]->vertices();
      SDL_Point points[vertices.size() + 1];
      std::vector<Vec2> axes = (*map->solids())[j]->bounds()->normals();
      // draw the first point and normal
      {
        // drawing a normal
        Position start = (vertices[0] + vertices[vertices.size() - 1]) * 0.5;
        Position end = start + axes[0];
        SDL_Point line[2];
        line[0] = camera.screen_transform(start);
        line[1] = camera.screen_transform(end);
        SDL_RenderDrawLines(renderer, line, 2);
        // drawing a point
        points[0] = camera.screen_transform(vertices[0]);
        fill_circle(renderer, points[0].x, points[0].y, 3);
      }
      // draw the remaining points, edges, and and normals
      for(unsigned int i = 1; i < axes.size(); i++) {
        // drawing a normal
        Position start = (vertices[i] + vertices[i - 1]) * 0.5;
        Position end = start + axes[i];
        SDL_Point line[2];
        line[0] = camera.screen_transform(start);
        line[1] = camera.screen_transform(end);
        SDL_RenderDrawLines(renderer, line, 2);
        // drawing a point
        points[i] = camera.screen_transform(vertices[i]);
        fill_circle(renderer, points[i].x, points[i].y, 3);
      }
      // drawing the sides
      points[vertices.size()] = points[0];
      SDL_RenderDrawLines(renderer, points, vertices.size() + 1);
      // drawing the "might collide" box
      SDL_Rect rect = camera.screen_transform((*map->solids())[j]);
      SDL_RenderDrawRect(renderer, &rect);
    }

    // draw entities
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    std::vector<Entity*> entities;
    entities = world.entities_in_region(*camera.region);
    for(Entity* e : entities) {
      SDL_Rect rect;
      rect = camera.screen_transform(e->region);
      // draw the player as a red square instead of a white circle
      if(e == &player) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      } else {
        fill_circle(renderer, rect.x + rect.w/2, rect.y + rect.h/2, rect.w/2);
      }
      if(e->max_health != 0) {
        SDL_Rect full_hp_bar = {rect.x, rect.y - 10, rect.w, 5};
        SDL_Rect hp_bar = {rect.x, rect.y - 10,
                           (int)(rect.w * e->health / e->max_health), 5};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &full_hp_bar);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &hp_bar);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      }
    }

    // draw projectiles
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(Projectile p : *world.projectiles()) {
      SDL_Rect rect;
      Region* r = new Region(p.position, new CircularBounds(0.1));
      rect = camera.screen_transform(r);
      fill_circle(renderer, rect.x + rect.w/2, rect.y + rect.h/2, rect.w/2);
    }

    // finished rendering cycle
    SDL_RenderPresent(renderer);
    SDL_Delay(16); // delay added so that the processor doesnt overheat :)
    prev_ticks = curr_ticks;
  }

  // close everything sanely
  essentia::shutdown();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  Mix_FreeMusic(music);
  Mix_Quit();
  SDL_Quit();
}
