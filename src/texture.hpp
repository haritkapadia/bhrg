#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <map>
#include <memory>
#include <string>

class Texture {
  private:
    SDL_Texture *t;
    SDL_Renderer *renderer;
    static std::map<std::string, Texture> global_textures;

  public:
    Texture(SDL_Renderer *renderer, std::string image_path, int x, int y);
    Texture(SDL_Renderer *renderer, std::string image_path);
    Texture(SDL_Renderer *renderer, std::string text, TTF_Font *font, SDL_Color color, int x,
            int y);
    Texture(SDL_Renderer *renderer, std::string text, TTF_Font *font, SDL_Color color);
    Texture(Texture &&other);
    ~Texture();
    SDL_Rect rect;
    void draw();
    void draw(SDL_Point p);
    static void draw_text(SDL_Renderer *renderer, std::string text, TTF_Font *font, SDL_Color color,
                          int x, int y);
    static void create_global_texture(std::string name, Texture *t);
    static Texture *global_texture(std::string name);
    friend std::ostream &operator<<(std::ostream &os, Texture const &l);
};

#endif
