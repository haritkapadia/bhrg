#include "texture.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialise SDL. ";
        std::cerr << "SDL_Error: " << SDL_GetError() << '\n';
    }
    SDL_Window *window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          500, 500, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "Could not create window. ";
        std::cerr << "SDL_Error: " << SDL_GetError() << '\n';
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        std::cerr << "Could not create image loader. ";
        std::cerr << "IMG_Error: " << IMG_GetError() << '\n';
    }
    // Texture::create_global_texture("Player", Texture(renderer, "img\\player.png"));
    // Texture::global_texture("Player")->draw();
    SDL_SetRenderDrawColor(renderer, 200, 100, 50, 255);
    SDL_Rect rect = {100, 100, 20, 20};
    SDL_RenderFillRect(renderer, &rect);
    Texture(renderer, "img\\player.png").draw();
    SDL_RenderPresent(renderer);
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "Done.\n";
    return 0;
}
