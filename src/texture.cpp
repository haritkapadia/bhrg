#include "texture.hpp"
#include "debug.hpp"

std::map<std::string, Texture> Texture::global_textures;

// Texture::Texture(SDL_Renderer *renderer) {
//     this->renderer = renderer;
//     t = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STATIC, 0, 0);
//     rect = {0, 0, 0, 0};
// }

Texture::Texture(SDL_Renderer *renderer, std::string image_path, int x, int y) {
    this->renderer = renderer;
    SDL_Surface *s = IMG_Load(image_path.c_str());
    if (!s)
        debug::print("IMG Load error", IMG_GetError());
    t = SDL_CreateTextureFromSurface(renderer, s);
    if (!t)
        debug::print("Texture creation error", SDL_GetError());
    rect = {x, y, s->w, s->h};
    SDL_FreeSurface(s);
}

Texture::Texture(SDL_Renderer *renderer, std::string image_path)
    : Texture(renderer, image_path, 0, 0) {}

Texture::Texture(SDL_Renderer *renderer, std::string text, TTF_Font *font, SDL_Color color, int x,
                 int y) {
    this->renderer = renderer;
    SDL_Surface *s = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!s)
        debug::print("TTF Render error", TTF_GetError());
    t = SDL_CreateTextureFromSurface(renderer, s);
    if (!t)
        debug::print("Texture creation error", SDL_GetError());
    rect = {x, y, s->w, s->h};
    SDL_FreeSurface(s);
}

Texture::Texture(SDL_Renderer *renderer, std::string text, TTF_Font *font, SDL_Color color)
    : Texture(renderer, text, font, color, 0, 0) {}

Texture::Texture(Texture &&other)
    : renderer(std::move(other.renderer)), t(std::exchange(other.t, nullptr)) {}

Texture::~Texture() {
    SDL_DestroyTexture(t);
    t = nullptr;
}

void Texture::draw(SDL_Point p) {
    SDL_Point old = {rect.x, rect.y};
    rect.x = p.x;
    rect.y = p.y;
    draw();
    rect.x = old.x;
    rect.y = old.y;
}

void Texture::draw() {
    int error = SDL_RenderCopy(renderer, t, NULL, &rect);
    if (error)
        debug::print(t, "Drawing error", SDL_GetError());
}

void Texture::draw_text(SDL_Renderer *renderer, std::string text, TTF_Font *font, SDL_Color color,
                        int x, int y) {
    Texture t(renderer, text, font, color, x, y);
    t.draw();
}

void Texture::create_global_texture(std::string name, Texture *t) {
    global_textures.insert({name, std::move(*t)});
}

Texture *Texture::global_texture(std::string name) { return &global_textures.at(name); }

std::ostream &operator<<(std::ostream &os, Texture const &l) {
    return os << "Texture[" << l.rect.x << ' ' << l.rect.y << ' ' << l.rect.w << ' ' << l.rect.h
              << "]";
}
