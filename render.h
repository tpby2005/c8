#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>

#include "chip.h"

typedef struct Render
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Render;

void render_init(Render *render);

void render_quit(Render *render);

void render_update(Render *render, Chip8 *chip);

bool render_input(uint8_t *keys);

#endif // RENDER_H