// UNORDERED_SET IN WORLD PROJECTILES KILLS THE PROGRAM AHHHH!!!!

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include "bounds.hpp"
#include "camera.hpp"
#include "debug.hpp"
#include "effect.hpp"
#include "entity.hpp"
#include "map.hpp"
#include "spell.hpp"
#include "timeline.hpp"
#include "vec.hpp"
#include "world.hpp"

#define SAMPLE_RATE 44100
// using namespace essentia;

/**
 * Places a filled circle onto the renderer.
 *
 * @param renderer   The renderer.
 * @param cx         The screen x-coordinate of the center of the circle.
 * @param cy         The screen y-coordinate of the center of the circle.
 * @param radius     The radius of the circle in pixels
 */

TTF_Font *FONT = NULL;

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
    Vec2 camera_position = camera->occupies.region->position;
    Vec2 camera_size = camera->occupies.region->bounds()->size();
    // j is calculated for width here
    if (camera->occupies.region->position.x - camera_size.x / 2 > 0)
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

void write_text(SDL_Renderer *renderer, int x, int y, SDL_Color color, std::string text) {
    SDL_Surface *s = TTF_RenderText_Solid(FONT, text.c_str(), color);
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_Rect r = {x, y, s->w, s->h};
    SDL_RenderCopy(renderer, t, NULL, &r);
}

void draw_timeline(SDL_Renderer *renderer, Timeline *timeline, SDL_Rect bounds,
                   long long framewidth, long long start, long long interval) {
    SDL_Rect rect = bounds;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0x64, 0xb5, 0xf6, 255);
    rect = {bounds.x, bounds.y, bounds.w, 15};
    SDL_RenderFillRect(renderer, &rect);
    for (long long i = (start + (start % interval)) % interval; i < framewidth; i += interval) {
        int line_x = (int)(bounds.x + i * bounds.w / framewidth);
        SDL_RenderDrawLine(renderer, line_x, bounds.y, line_x, bounds.y + bounds.h);
        // I would use std::to_string but that function is broken on my MinGW compiler
        char numtext[31];
        std::sprintf(numtext, "%lld", start + i);
        write_text(renderer, line_x, bounds.y, {0, 0, 0}, numtext);
    }
    SDL_SetRenderDrawColor(renderer, 0xe3, 0x33, 0x71, 255);
    for (Event *e : timeline->future) {
        int line_x = (int)(bounds.x + (e->start - start) * bounds.w / framewidth);
        if (line_x >= 0 && line_x <= bounds.x + bounds.w) {
            rect = {line_x - 2, bounds.y + 20, 5, 5};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    {
        int line_x = (int)(bounds.x + (timeline->elapsed() - start) * bounds.w / framewidth);
        SDL_RenderDrawLine(renderer, line_x, bounds.y + 15, line_x, bounds.y + bounds.h);
    }
}

template <typename T> T Constrain(T val, T low, T high) {
    if (val < low)
        return low;
    if (val > high)
        return high;
    return val;
}

void draw_events(SDL_Renderer *renderer, Timeline *timeline, Camera *camera, int mx, int my) {
    for (Event *_e : timeline->future) {
        unsigned int event_type = std::distance(
            Timeline::KNOWN_EVENT.begin(),
            std::find(Timeline::KNOWN_EVENT.begin(), Timeline::KNOWN_EVENT.end(), _e->name));
        switch (event_type) {
        case 1: {
            SpawnEnemy1 *e = dynamic_cast<SpawnEnemy1 *>(_e);
            SDL_Rect rect;
            rect = camera->screen_transform(e->_enemy->e.occupies.region);
            SDL_SetRenderDrawColor(
                renderer, 0xff, 0xb7, 0x4d,
                (1000 -
                 Constrain(std::abs((long long)(timeline->elapsed() - e->start)), 0ll, 1000ll)) *
                    255 / 1000);
            SDL_RenderFillRect(renderer, &rect);
            if (mx >= rect.x && mx < rect.x + rect.w && my >= rect.y && my < rect.y + rect.h) {
                SDL_Color black = {0, 0, 0};
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                rect = {mx + 7, my, 100, 50};
                SDL_RenderFillRect(renderer, &rect);
                char numtext[100] = "Start: ";
                std::sprintf(numtext + 7, "%lld", e->start);
                write_text(renderer, rect.x + 5, rect.y + 5, black, numtext);
                std::strcpy(numtext, "Duration: ");
                std::sprintf(numtext + 10, "%llu", e->duration);
                write_text(renderer, rect.x + 5, rect.y + 15, black, numtext);
                std::strcpy(numtext, "Speed: ");
                std::sprintf(numtext + 7, "%.3llf", e->_enemy->e.moves.speed);
                write_text(renderer, rect.x + 5, rect.y + 25, black, numtext);
            }
            break;
        }
        default:
            break;
        };
    }
}

bool fexists(const std::string &filename) {
    std::ifstream ifile(filename.c_str());
    return (bool)ifile;
}

int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Mix_Music *music; // Music playing for SDL
    bool running = false;
    int SCREEN_WIDTH = 500, SCREEN_HEIGHT = 500;

    // SOUND STUFF
    std::string music_file = "./music/mt. fujitive - trees.mp3";
    double bpm = 100;
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
    window = SDL_CreateWindow("Bullet Hell Rhythm Game", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT + 50,
                              SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "Could not create window. ";
        std::cerr << "SDL_Error: " << SDL_GetError() << '\n';
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (TTF_Init() < 0) {
        std::cerr << "Could not create TTF loader. ";
        std::cerr << "TTF_Error: " << TTF_GetError() << '\n';
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    FONT = TTF_OpenFont("calibri.ttf", 12);
    if (FONT == NULL) {
        std::cerr << "Could not load font. ";
        std::cerr << "TTF_Error: " << TTF_GetError() << '\n';
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Could not create mixer. ";
        std::cerr << "Mix_Error: " << Mix_GetError() << '\n';
    }
    music = Mix_LoadMUS(music_file.c_str());
    if (music == NULL) {
        std::cerr << "Could not load music. ";
        std::cerr << "Mix_Error: " << Mix_GetError() << '\n';
    }

    running = true;
    Mix_PlayMusic(music, 0);
    unsigned long long prog_start = SDL_GetTicks();
    unsigned long long prev_ticks, curr_ticks;
    // These values are used to change visuals according to the analysed song
    unsigned int tempo_color = 0, beat_color = 0;
    unsigned long long ibeat = 0;
    double spb = 0;
    // The playing region is loaded here

    Timeline timeline;
    EntityFactory *_player = new EntityFactory();
    _player->lives({true, 100, 100})
        ->moves({Vec2::zero, 10})
        ->occupies({new PolygonRegion(Vec2::zero, new RectangularBounds({1, 1}))});
    World world(_player->create(), &timeline);
    delete _player;
    Entity *player = world.player;
    player->name = "player";

    Timeline update_timer;
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

    prev_ticks = SDL_GetTicks();
    Camera camera = Camera(&world, Vec2(0, 0), new RectangularBounds(Vec2(10, 10)), &SCREEN_WIDTH,
                           &SCREEN_HEIGHT);

    std::vector<Position> vertices;
    int vertices_size = 0;
    std::vector<PolygonRegion *> *solids = map->solids();
    bool ctrl_press = false;
    bool kup, kdown, kleft, kright;
    kup = kdown = kleft = kright = false;
    bool show_grid = false;
    long long interval = 1000 / bpm;
    long long start = 0;
    long long framewidth = 8 * interval;
    timeline.start(SDL_GetTicks());
    timeline.pause(SDL_GetTicks());
    update_timer.start(SDL_GetTicks());
    // Main event loop
    while (running) {
        curr_ticks = SDL_GetTicks();
        SDL_Event e;
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        // screen mouse coordinates transformed to world coordinates
        Vec2 mtarget;
        {
            double target_x = (camera.occupies.region->bounds()->size().x / SCREEN_WIDTH) *
                              (mx +
                               camera.occupies.region->position.x * SCREEN_WIDTH /
                                   camera.occupies.region->bounds()->size().x -
                               SCREEN_WIDTH / 2);
            double target_y = (camera.occupies.region->bounds()->size().y / SCREEN_HEIGHT) *
                              (-my +
                               camera.occupies.region->position.y * SCREEN_HEIGHT /
                                   camera.occupies.region->bounds()->size().y +
                               SCREEN_HEIGHT / 2);
            mtarget = {target_x, target_y};
        }

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (my < SCREEN_HEIGHT) {
                    Vec2 new_point = mtarget;
                    if (!ctrl_press)
                        new_point = {std::round(mtarget.x), std::round(mtarget.y)};
                    if ((int)vertices.size() <= vertices_size)
                        vertices.push_back(new_point);
                    else
                        vertices[vertices_size] = new_point;
                    vertices_size += 1;
                } else {
                    long long elapsed;
                    std::cin >> elapsed;
                    timeline.now = timeline.start_time + timeline.total_paused + elapsed;
                }
            } else if (e.type == SDL_MOUSEWHEEL) {
                if (my < SCREEN_HEIGHT) {
                    if (e.wheel.y < 0) {
                        camera.zoom(0.8);
                    } else if (e.wheel.y > 0) {
                        camera.zoom(1.25);
                    }
                } else {
                    if (e.wheel.y < 0) {
                        framewidth += ctrl_press ? (1000 / bpm) : interval;
                    } else if (e.wheel.y > 0) {
                        framewidth -= ctrl_press ? (1000 / bpm) : interval;
                        if (framewidth < 2 * interval)
                            framewidth = 2 * interval;
                    }
                }
            } else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
                switch (e.key.keysym.sym) {
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
                    for (Position p : vbounds) {
                        std::cout << l << ' ' << r << ' ' << u << ' ' << d << '\n';
                        if (p.x < l)
                            l = p.x;
                        if (p.x > r)
                            r = p.x;
                        if (p.y < d)
                            d = p.y;
                        if (p.y > u)
                            u = p.y;
                    }
                    std::cout << "Done: " << l << ' ' << r << ' ' << u << ' ' << d << "\n\n";
                    const Position center = {(l + r) / 2, (u + d) / 2};
                    for (auto it = vbounds.begin(); it != vbounds.end(); it++) {
                        it->x -= center.x;
                        it->y -= center.y;
                    }
                    solids->push_back(
                        new PolygonRegion(center, new ConvexBounds(vbounds, {r - l, u - d})));
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
                    // toggles grid
                case SDLK_k:
                    interval += 1000 * (ctrl_press ? 1 : 10) / bpm;
                    break;
                case SDLK_j:
                    interval -= 1000 * (ctrl_press ? 1 : 10) / bpm;
                    if (interval <= 0)
                        interval = 1000 / bpm;
                    break;
                case SDLK_h:
                    start -= ctrl_press ? (1000 / bpm) : interval;
                    break;
                case SDLK_l:
                    start += ctrl_press ? (1000 / bpm) : interval;
                    break;
                case SDLK_COMMA:
                    timeline.now -= ctrl_press ? (1000 / bpm) : interval;
                    break;
                case SDLK_PERIOD:
                    timeline.now += ctrl_press ? (1000 / bpm) : interval;
                    break;
                case SDLK_MINUS:
                    framewidth -= ctrl_press ? (1000 / bpm) : interval;
                    if (framewidth < 2 * interval)
                        framewidth = 2 * interval;
                    break;
                case SDLK_EQUALS:
                    framewidth += ctrl_press ? (1000 / bpm) : interval;
                    break;
                case SDLK_g:
                    show_grid = !show_grid;
                    break;
                case SDLK_1: {
                    EntityFactory *_enemy = new EntityFactory();
                    _enemy->lives({true, 200, 200})
                        ->moves({Vec2::zero, 3})
                        ->occupies({new PolygonRegion(mtarget, new RectangularBounds({1, 1}))});
                    timeline.add(new SpawnEnemy1(timeline.elapsed(), 10000, &world, _enemy));
                    break;
                }
                default:
                    break;
                }
            } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
                switch (e.key.keysym.sym) {
                case SDLK_LCTRL:
                    // falls through
                case SDLK_RCTRL:
                    ctrl_press = false;
                    break;
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
                SCREEN_HEIGHT -= 50;
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

        update_timer.update_now(SDL_GetTicks());
        update_timer.process();
        camera.occupies.region->position = player->occupies.region->position;

        // change colour of beat box based on bpm
        unsigned long long now = SDL_GetTicks() - prog_start;
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

        // moves players, projectiles, kills dead entities, etc.
        // world.clean_up();
        player->move(update_timer.diff() / 1000.0);

        // clear screen, beginning the drawing cycle
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

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
        for (unsigned int j = 0; j < map->solids()->size(); j++) {
            // each shape has a new colour
            unsigned int col = (128 + j * 25) % 255;
            SDL_SetRenderDrawColor(renderer, col, col / 2, (col * 2) % 255, 255);
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
            for (unsigned int i = 1; i < axes.size(); i++) {
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
        // drawing current construction
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_Point points[vertices_size + 1];
            for (int i = 0; i < vertices_size; i++) {
                points[i] = camera.screen_transform(vertices[i]);
                fill_circle(renderer, points[i].x, points[i].y, 7);
            }
            points[vertices_size] = {mx, my};
            SDL_RenderDrawLines(renderer, points, vertices_size + 1);
        }

        // draw entities
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        std::vector<Entity *> entities = world.entities_in_region(camera.occupies.region);
        for (Entity *e : entities) {
            SDL_Rect rect;
            rect = camera.screen_transform(e->occupies.region);
            // draw the player as a red crosshair instead of a white circle
            if (&(*e) == player) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                rect = {(SCREEN_WIDTH - 10) / 2, (SCREEN_HEIGHT - 2) / 2, 10, 2};
                SDL_RenderFillRect(renderer, &rect);
                rect = {(SCREEN_WIDTH - 2) / 2, (SCREEN_HEIGHT - 10) / 2, 2, 10};
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            } else {
                fill_circle(renderer, rect.x + rect.w / 2, rect.y + rect.h / 2, rect.w / 2);
            }
            // if (e->is_comp[Entity::LIVES] != 0) {
            //     SDL_Rect full_hp_bar = {rect.x, rect.y - 10, rect.w, 5};
            //     SDL_Rect hp_bar = {
            //         rect.x, rect.y - 10,
            //         (int)(rect.w * e->lives.health / e->lives.max_health),
            //         5};
            //     SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            //     SDL_RenderFillRect(renderer, &full_hp_bar);
            //     SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            //     SDL_RenderFillRect(renderer, &hp_bar);
            //     SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            // }
        }

        // draw projectiles
        SDL_SetRenderDrawColor(renderer, 25, 80, 255, 255);
        for (Projectile p : world.projectiles) {
            SDL_Rect rect;
            Region *r = new Region(p.position, new CircularBounds(0.1));
            rect = camera.screen_transform(r);
            fill_circle(renderer, rect.x + rect.w / 2, rect.y + rect.h / 2, rect.w / 2);
        }

        draw_timeline(renderer, &timeline, {0, SCREEN_HEIGHT, SCREEN_WIDTH, 50}, framewidth, start,
                      interval);
        draw_events(renderer, &timeline, &camera, mx, my);

        // finished rendering cycle
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // delay added so that the processor doesnt overheat :)
        prev_ticks = curr_ticks;
    }

    {
        std::ofstream mapfile;
        mapfile.open(argv[1], std::ios::binary);
        map->write(&mapfile);
        mapfile.close();
        std::ofstream timefile;
        timefile.open(argv[2], std::ios::binary);
        timeline.write(&timefile);
        timefile.close();
    }

    // close everything sanely
    TTF_CloseFont(FONT);
    Mix_FreeMusic(music);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
    return 0;
}
