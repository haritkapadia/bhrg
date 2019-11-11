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

#include "entity.hpp"
#include "world.hpp"
#include "spell.hpp"

#define SAMPLE_RATE 44100

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

void analyse_song(std::string filename, essentia::Real* bpm, std::vector<essentia::Real>* ticks) {
  essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
  essentia::standard::Algorithm* audio = factory.create("MonoLoader",
                                                        "filename", filename,
                                                        "sampleRate", SAMPLE_RATE);
  essentia::standard::Algorithm* rhythm_extractor = factory.create("RhythmExtractor2013");
  std::vector<essentia::Real> audio_buffer;
  essentia::Real confidence;
  std::vector<essentia::Real> estimates;
  std::vector<essentia::Real> bpm_intervals;
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
    std::cout << "Song analysis took " << (end - start) / 1000.0 << " seconds.\n";
  }

  
  
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Could not initialise SDL. SDL_Error: " << SDL_GetError() << '\n';
  }
  if((window = SDL_CreateWindow("Bullet Hell Rhythm Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 500, 500, SDL_WINDOW_SHOWN)) == NULL) {
    std::cerr << "Could not create window. SDL_Error: " << SDL_GetError() << '\n';
  }
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "Could not create window. Mix_Error: " << Mix_GetError() << '\n';
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if((music = Mix_LoadMUS(music_file.c_str())) == NULL) {
    std::cerr << "Could not load music. Mix_Error: " << Mix_GetError() << '\n';
  }
  

  running = true;
  Mix_PlayMusic(music, 0);
  unsigned long long start = SDL_GetTicks();
  unsigned long long prev_ticks, curr_ticks;
  unsigned int tempo_color = 0, beat_color = 0;
  unsigned long long ibeat = 0;
  double spb = 0;
  World world;
  world.spawn(new Player(0.5, 0.5));
  Spell relocate = Spell(&world, Spell::Type::TARGET);
  prev_ticks = SDL_GetTicks();
  while(running) {
    curr_ticks = SDL_GetTicks();
    SDL_Event e;
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    double target_x = mx/250.0 - 1;
    double target_y = 1 - my/250.0;
    while(SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_KEYDOWN) {
        switch(e.key.keysym.sym) {
        case SDLK_ESCAPE:
          running = false;
          break;
        case SDLK_w:
          world.player(0)->vy(0.5);
          break;
        case SDLK_s:
          world.player(0)->vy(-0.5);
          break;
        case SDLK_a:
          world.player(0)->vx(-0.5);
          break;
        case SDLK_d:
          world.player(0)->vx(0.5);
          break;
        case SDLK_PERIOD:
          relocate.use(world.player(0), target_x, target_y);
          std::cout << "entities at ";
          for(auto e : world.entities_in_bounds(Bounds(-1, -1, 2, 2)))
            std::cout << *e << ' ';
          std::cout << '\n';
          break;
        case SDLK_SPACE:
          {
            Bullet* bullet = new Bullet(world.player(0)->x(), world.player(0)->y());
            bullet->vx(target_x - bullet->x());
            bullet->vy(target_y - bullet->y());
            world.spawn(bullet);
          }
          break;
        default:
          break;
        }
      } else if(e.type == SDL_KEYUP) {
        switch(e.key.keysym.sym) {
        case SDLK_w:
          world.player(0)->vy(0);
          break;
        case SDLK_s:
          world.player(0)->vy(0);
          break;
        case SDLK_a:
          world.player(0)->vx(0);
          break;
        case SDLK_d:
          world.player(0)->vx(0);
          break;
        default:
          break;
        }
      } else if(e.type == SDL_QUIT) {
        running = false;
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

    for(MovingEntity* e : *world.moving_entities()) {
      e->move((curr_ticks - prev_ticks) / 1000.0);
    }

    SDL_Rect rect;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, tempo_color, 50, tempo_color, 255);
    rect = {0, 0, 250, 500};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, beat_color, 100, beat_color, 255);
    rect = {250, 0, 250, 500};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    for(Entity* e : world.entities_in_bounds(Bounds(-1, -1, 2, 2))) {
      if(e == world.player(0))
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      fill_circle(renderer, 250 + 250 * e->x(), 250 - 250 * e->y(), 10);
      if(e == world.player(0))
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
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
