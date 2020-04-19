#ifndef __AURUM_SDL_HELPER__
#define __AURUM_SDL_HELPER__

#include <SDL2/SDL.h>

#include <vector>

extern std::vector<SDL_Event> sdlEvents;

void useSDL();
void freeSDL();

SDL_Event* getEvent(uint32_t);

#endif