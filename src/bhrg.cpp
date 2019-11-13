#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <essentia/algorithmfactory.h>
#include <essentia/pool.h>

#include "bounds.hpp"
#include "entity.hpp"
#include "world.hpp"
#include "spell.hpp"

#define SAMPLE_RATE 44100
using namespace essentia;

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
  audio->output("audio").set(audio_buffer);
  audio->compute();
  rhythm_extractor->input("signal").set(audio_buffer);
  rhythm_extractor->output("bpm").set(*bpm);
  rhythm_extractor->output("ticks").set(*ticks);
  rhythm_extractor->output("confidence").set(confidence);
  rhythm_extractor->output("estimates").set(estimates);
  rhythm_extractor->output("bpmIntervals").set(bpm_intervals);
  rhythm_extractor->compute();
  delete audio;
  delete rhythm_extractor;
}

int main(int argc, char* argv[]) {
  SDL_Window* window;
  SDL_Renderer* renderer;
  Mix_Music* music;
  bool running;


  // SOUND STUFF
  std::string music_file = "./music/mt. fujitive - trees.mp3";
  essentia::init();
  essentia::Pool pool;
  essentia::Real bpm;
  std::vector<essentia::Real> ticks;
  {
    unsigned long long start = SDL_GetTicks();
    analyse_song(music_file, &bpm, &ticks);
    unsigned long long end = SDL_GetTicks();
    std::cout << "Song analysis took " << (end - start) / 1000. << " seconds.\n";
  }



  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Could not initialise SDL. SDL_Error: " << SDL_GetError() << '\n';
  }
  window = SDL_CreateWindow("Bullet Hell Rhythm Game",
                            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                            500, 500,
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
  int SCREEN_WIDTH = 500, SCREEN_HEIGHT = 500;
  SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
  unsigned long long start = SDL_GetTicks();
  unsigned long long prev_ticks, curr_ticks;
  unsigned int tempo_color = 0, beat_color = 0;
  unsigned long long ibeat = 0;
  double spb = 0;
  World world;
  Player player = Player({0, 0}, new RectangularBounds(0.05, 0.05));
  world.spawn(&player);
  Spell relocate = Spell(&world, Spell::Type::TARGET);
  prev_ticks = SDL_GetTicks();
  while(running) {
    curr_ticks = SDL_GetTicks();
    SDL_Event e;
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    double target_x = (double)mx/(SCREEN_WIDTH / 2) - 1;
    double target_y = 1 - (double)my/(SCREEN_HEIGHT / 2);
    while(SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
        case SDLK_ESCAPE:
          running = false;
          break;
        case SDLK_w:
          player.vy(player.vy() + 0.5);
          break;
        case SDLK_s:
          player.vy(player.vy() - 0.5);
          break;
        case SDLK_a:
          player.vx(player.vx() - 0.5);
          break;
        case SDLK_d:
          player.vx(player.vx() + 0.5);
          break;
        case SDLK_PERIOD:
          relocate.use(&player, target_x, target_y);
          break;
        case SDLK_SPACE:
          {
            Bullet* b = new Bullet(player.position(), new CircularBounds(0.01));
            b->vx(target_x - b->position().x);
            b->vy(target_y - b->position().y);
            world.spawn(b);
          }
          break;
        default:
          break;
        }
      } else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
        case SDLK_w:
          player.vy(player.vy() - 0.5);
          break;
        case SDLK_s:
          player.vy(player.vy() + 0.5);
          break;
        case SDLK_a:
          player.vx(player.vx() + 0.5);
          break;
        case SDLK_d:
          player.vx(player.vx() - 0.5);
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

    unsigned long long now = SDL_GetTicks() - start;
    if(now >= spb * 1000) {
      tempo_color = (tempo_color + 15) % 255;
      spb += 60.0 / bpm;
    }
    if(now >= ticks[ibeat] * 1000) {
      beat_color = (beat_color + 30) % 255;
      ibeat += 1;
      if(ibeat >= ticks.size())
        break;
    }

    for(MovingEntity* e : *world.moving_entities())
      e->move((curr_ticks - prev_ticks) / 1000.0);

    SDL_Rect rect;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, tempo_color, 50, tempo_color, 255);
    rect = {0, 0, (SCREEN_WIDTH / 2), SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, beat_color, 100, beat_color, 255);
    rect = {(SCREEN_WIDTH / 2), 0, (SCREEN_WIDTH / 2), SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    auto ents = world.entities_in_area({-1, -1}, new RectangularBounds(2, 2));
    for(Entity* e : ents) {
      Bounds* b = e->bounds();
      if(e == &player) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        RectangularBounds* p = dynamic_cast<RectangularBounds*>(b);
        SDL_Rect rect =
          {
           (int)((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 2) * e->position().x),
           (int)((SCREEN_HEIGHT / 2) - (SCREEN_HEIGHT / 2) * e->position().y),
           (int)(SCREEN_WIDTH * p->length()),
           (int)(SCREEN_HEIGHT * p->width())
          };
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      } else {
        fill_circle(renderer,
                    (SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 2) * e->position().x,
                    (SCREEN_HEIGHT / 2) - (SCREEN_HEIGHT / 2) * e->position().y,
                    SCREEN_WIDTH * dynamic_cast<CircularBounds*>(b)->radius());
      }
    }
    SDL_RenderPresent(renderer);
    prev_ticks = curr_ticks;
  }


  essentia::shutdown();


  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  Mix_FreeMusic(music);
  Mix_Quit();
  SDL_Quit();
}
