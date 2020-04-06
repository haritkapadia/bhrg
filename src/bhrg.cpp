// UNORDERED_SET IN WORLD PROJECTILES KILLS THE PROGRAM AHHHH!!!!

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
// #include <essentia/algorithmfactory.h>
// #include <essentia/pool.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include "camera.hpp"
#include "debug.hpp"
#include "effect.hpp"
#include "entity.hpp"
#include "map.hpp"
#include "region.hpp"
#include "spell.hpp"
#include "timeline.hpp"
#include "vec.hpp"
#include "world.hpp"

#define SAMPLE_RATE 44100
// using namespace essentia;

void draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius) {
    for (int i = -radius; i <= radius; i++) {
        int height = (int)std::sqrt(radius * radius - i * i);
        SDL_RenderDrawPoint(renderer, cx + i, cy - height);
        SDL_RenderDrawPoint(renderer, cx + i, cy + height);
    }
}

/**
 * Places a filled circle onto the renderer.
 *
 * @param renderer   The renderer.
 * @param cx         The screen x-coordinate of the center of the circle.
 * @param cy         The screen y-coordinate of the center of the circle.
 * @param radius     The radius of the circle in pixels
 */
void fill_circle(SDL_Renderer *renderer, int cx, int cy, int radius) {
    for (int i = -radius; i <= radius; i++) {
        int height = std::sqrt(radius * radius - i * i);
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
void draw_grid(SDL_Renderer *renderer, Camera *camera, int width, int height) {
    // j contains the leftmost integer grid position
    int j = 0;
    double gp = 0;
    Vec2 camera_position = camera->occupies.region->center();
    Vec2 camera_size = camera->occupies.region->size();
    // j is calculated for width here
    if (camera_position.x - camera_size.x / 2 > 0)
        j = std::ceil(camera_position.x - camera_size.x / 2);
    else
        j = 1 + std::floor(camera_position.x - camera_size.x / 2);
    // draws the vertical lines for the grid from the left of the screen
    // this happens by transforming the left bounds of the camera position
    // to screen position and then moving it to the centre
    gp = (width / camera_size.x) * (j - camera_position.x) + width / 2;
    for (int i = 0; i < camera_size.x; i++) {
        SDL_RenderDrawLine(renderer, (int)gp, 0, (int)gp, height);
        // note that since the grid width is consistent,
        // the position can increase linearly
        gp += width / camera_size.x;
    }
    // j is calculated for height here
    if (camera_position.y - camera_size.y / 2 > 0)
        j = std::ceil(camera_position.y - camera_size.y / 2);
    else
        j = 1 + std::floor(camera_position.y - camera_size.y / 2);
    // draws the horizontal lines for the grid from the bottom of the scren
    // gp is calculated similarly as for the vertical lines
    gp = height / 2 - (height / camera_size.y) * (j - camera_position.y);
    for (int i = 0; i < camera_size.y; i++) {
        SDL_RenderDrawLine(renderer, 0, gp, width, gp);
        gp -= height / camera_size.y;
    }
}

// undocumented function
// This function will likely be rewritten and moved to a different file
// Analyses song using essentia, specifically finding beat markings and bpm
/*
void analyse_song(std::string filename, Real *bpm, std::vector<Real> *ticks) {
    standard::AlgorithmFactory &factory =
        standard::AlgorithmFactory::instance();
    standard::Algorithm *audio = factory.create(
        "MonoLoader", "filename", filename, "sampleRate", SAMPLE_RATE);
    standard::Algorithm *rhythm_extractor =
        factory.create("RhythmExtractor2013");
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
    rhythm_extractor->compute(); // get values and get out
    delete audio;
    delete rhythm_extractor;
}
*/

bool fexists(const std::string &filename) {
    std::ifstream ifile(filename.c_str());
    return (bool)ifile;
}

// argc, argv not used
int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Mix_Music *music; // Music playing for SDL
    bool running = false;
    int SCREEN_WIDTH = 500, SCREEN_HEIGHT = 500;

    debug::print("I love debugging!");

    // SOUND STUFF
    std::string music_file =
        "./music/The_Polish_Ambassador__Nitty_Scott_-_02_-_Chill_or_Be_Chilled.mp3";
    // essentia::init();
    // essentia::Pool pool;
    // essentia::Real bpm = 80;
    double bpm = 80;
    // std::vector<essentia::Real> ticks;
    std::vector<double> ticks;
    {
        unsigned long long start = SDL_GetTicks();
        // Song is currently ignored because analysis takes 10 seconds every run
        // analyse_song(music_file, &bpm, &ticks);
        unsigned long long end = SDL_GetTicks();
        std::cout << "Analysis took " << (end - start) / 1000. << " seconds.\n";
    }

    // initialising required SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialise SDL. ";
        std::cerr << "SDL_Error: " << SDL_GetError() << '\n';
    }
    window =
        SDL_CreateWindow("Bullet Hell Rhythm Game", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "Could not create window. ";
        std::cerr << "SDL_Error: " << SDL_GetError() << '\n';
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Could not create window. ";
        std::cerr << "Mix_Error: " << Mix_GetError() << '\n';
    }
    music = Mix_LoadMUS(music_file.c_str());
    if (music == NULL) {
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

    Timeline timeline;
    EntityFactory *_player = new EntityFactory();
    _player->lives({true, 100, 100})
        ->moves({Vec2::zero, 10})
        ->occupies({new Rectangle(Vec2::zero, {1, 1})});
    World world(_player->create(), &timeline);
    delete _player;
    Entity *player = world.player;
    player->name = "player";
    Map *map = &world.map;
    if (fexists(argv[1])) {
        std::ifstream mapfile;
        mapfile.open(argv[1], std::ios::binary);
        map->read(&mapfile);
        mapfile.close();
    }
    if (fexists(argv[2])) {
        std::ifstream timefile;
        timefile.open(argv[2], std::ios::binary);
        timeline.read(&timefile);
        timefile.close();
    }

    DamageOverTime *damage_event = new DamageOverTime({player}, 5000);
    Spell damage_spell;
    damage_spell.type = Spell::PROJECTILE;
    damage_spell.effects = {damage_event, new Speed({NULL}, 5000, 0.1)};
    damage_spell.source = player;
    Speed *speed_event = new Speed({player}, 5000, 3.0);
    Spell speed_spell;
    speed_spell.type = Spell::SELF;
    speed_spell.effects = {speed_event};
    speed_spell.source = player;
    Teleport *teleport_event = new Teleport({player});
    Spell teleport_spell;
    teleport_spell.type = Spell::POINT_TARGET;
    teleport_spell.effects = {teleport_event};
    teleport_spell.source = player;

    prev_ticks = SDL_GetTicks();
    Camera camera = Camera(new Rectangle(Vec2::zero, {10, 10}), &SCREEN_WIDTH, &SCREEN_HEIGHT);

    bool kup, kdown, kleft, kright;
    kup = kdown = kleft = kright = false;
    bool show_grid = false;
    timeline.start(SDL_GetTicks());
    // Main event loop
    while (running) {
        curr_ticks = SDL_GetTicks();
        SDL_Event e;
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        // screen mouse coordinates transformed to world coordinates
        Vec2 mtarget;
        {
            double target_x = (camera.occupies.region->size().x / SCREEN_WIDTH) *
                              (mx +
                               camera.occupies.region->center().x * SCREEN_WIDTH /
                                   camera.occupies.region->size().x -
                               SCREEN_WIDTH / 2);
            double target_y = (camera.occupies.region->size().y / SCREEN_HEIGHT) *
                              (-my +
                               camera.occupies.region->center().y * SCREEN_HEIGHT /
                                   camera.occupies.region->size().y +
                               SCREEN_HEIGHT / 2);
            mtarget = {target_x, target_y};
        }

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_MOUSEWHEEL) {
                if (e.wheel.y < 0) {
                    camera.zoom(0.8);
                } else if (e.wheel.y > 0) {
                    camera.zoom(1.25);
                }
            }
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
                switch (e.key.keysym.sym) {
                    // quit game
                case SDLK_ESCAPE:
                    running = false;
                    break;
                    // increases velocity upon WASD press
                case SDLK_w:
                    kup = true;
                    break;
                case SDLK_s:
                    kdown = true;
                    break;
                case SDLK_a:
                    kleft = true;
                    break;
                case SDLK_d:
                    kright = true;
                    break;
                    // adjust camera position using arrow keys
                case SDLK_UP:
                    camera.occupies.region->translate({0, camera.occupies.region->size().y / 10});
                    break;
                case SDLK_DOWN:
                    camera.occupies.region->translate({0, -camera.occupies.region->size().y / 10});
                    break;
                case SDLK_LEFT:
                    camera.occupies.region->translate({-camera.occupies.region->size().x / 10, 0});
                    break;
                case SDLK_RIGHT:
                    camera.occupies.region->translate({camera.occupies.region->size().x / 10, 0});
                    break;
                case SDLK_g:
                    show_grid = !show_grid;
                    break;
                case SDLK_1:
                    damage_spell.position = mtarget;
                    damage_spell.use(&world);
                    break;
                case SDLK_2:
                    speed_spell.use(&world);
                    break;
                case SDLK_3:
                    teleport_spell.position = mtarget;
                    teleport_spell.use(&world);
                    break;
                default:
                    break;
                }
            } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
                switch (e.key.keysym.sym) {
                case SDLK_w:
                    kup = false;
                    break;
                case SDLK_s:
                    kdown = false;
                    break;
                case SDLK_a:
                    kleft = false;
                    break;
                case SDLK_d:
                    kright = false;
                    break;
                default:
                    break;
                }
            } else if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_WINDOWEVENT) {
                // ensures the screen size is updated
                SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
            }
        }

        {
            Vec2 *v = &player->moves.velocity;
            if (!(kleft ^ kright)) {
                v->x = 0;
            } else if (kleft) {
                v->x -= 1;
            } else if (kright) {
                v->x += 1;
            }
            if (!(kup ^ kdown)) {
                v->y = 0;
            } else if (kdown) {
                v->y -= 1;
            } else if (kup) {
                v->y += 1;
            }
            *v = Vec2::normalize(*v);
        }

        timeline.update_now(SDL_GetTicks());
        timeline.process();

        // change colour of beat box based on bpm
        unsigned long long now = SDL_GetTicks() - start;
        if (now >= spb * 1000) {
            tempo_color = (tempo_color + 15) % 255;
            spb += 60.0 / bpm;
        }
        // change colour of beat box based on beat markers
        if (ticks.size() > 0) {
            if (now >= ticks[ibeat] * 1000) {
                beat_color = (beat_color + 30) % 255;
                ibeat += 1;
                if (ibeat >= ticks.size())
                    break;
            }
        }

        // clear screen, beginning the drawing cycle
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // moves players, projectiles, kills dead entities, etc.
        world.clean_up(renderer, &camera);

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
        if (show_grid) {
            SDL_SetRenderDrawColor(renderer, 63, 63, 63, 255);
            draw_grid(renderer, &camera, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        // draw solids
        for (unsigned int j = 0; j < map->solids.size(); j++) {
            // each shape has a new colour
            unsigned int col = (128 + j * 25) % 255;
            SDL_SetRenderDrawColor(renderer, col, col / 2, (col * 2) % 255, 255);
            // recording vertices, points, and axes
            // to draw   vertices, sides,  and normals
            if (auto s = dynamic_cast<Convex *>(map->solids[j])) {
                std::vector<Vec2> vertices = s->vertices();
                SDL_Point points[vertices.size() + 1];
                std::vector<Vec2> axes = s->normals();
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
                for (unsigned int i = 1; i < vertices.size(); i++) {
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
            } else if (auto s = dynamic_cast<Circle *>(map->solids[j])) {
                SDL_Rect rect = camera.screen_transform(s);
                SDL_Point center = camera.screen_transform(s->center());
                draw_circle(renderer, center.x, center.y, rect.w / 2);
            }
            // drawing the "might collide" box
            SDL_Rect rect = camera.screen_transform(map->solids[j]);
            SDL_RenderDrawRect(renderer, &rect);
        }

        // draw entities
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        std::vector<Entity *> entities;
        entities = world.entities_in_region(camera.occupies.region);
        for (Entity *e : entities) {
            SDL_Rect rect;
            rect = camera.screen_transform(e->occupies.region);
            // draw the player as a red square instead of a white circle
            if (e == player) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                // fill_circle(renderer, rect.x + rect.w / 2, rect.y + rect.h / 2, rect.w / 2);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            } else {
                fill_circle(renderer, rect.x + rect.w / 2, rect.y + rect.h / 2, rect.w / 2);
            }
            if (e->is_comp[Entity::LIVES] != 0) {
                SDL_Rect full_hp_bar = {rect.x, rect.y - 10, rect.w, 5};
                SDL_Rect hp_bar = {rect.x, rect.y - 10,
                                   (int)(rect.w * e->lives.health / e->lives.max_health), 5};
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &full_hp_bar);
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderFillRect(renderer, &hp_bar);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
        }

        // draw projectiles
        SDL_SetRenderDrawColor(renderer, 25, 80, 255, 255);
        for (Projectile p : world.projectiles) {
            SDL_Rect rect;
            Region *r = new Circle(p.position, 0.1);
            rect = camera.screen_transform(r);
            fill_circle(renderer, rect.x + rect.w / 2, rect.y + rect.h / 2, rect.w / 2);
        }

        // finished rendering cycle
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // delay added so that the processor doesnt overheat :)
        prev_ticks = curr_ticks;
    }

    // close everything sanely
    // essentia::shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_FreeMusic(music);
    Mix_Quit();
    SDL_Quit();
    return 0;
}
