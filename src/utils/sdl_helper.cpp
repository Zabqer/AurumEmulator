#include "sdl_helper.h"
#include "../log.h"

std::vector<SDL_Event> sdlEvents;

static size_t inUse;

void useSDL() {
		if (inUse++ == 0) {
				if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0) {
						logE("Error initializing SDL: " << SDL_GetError());
						abort();
				}
		}
}

void freeSDL() {
		 if (--inUse == 0) {
				SDL_Quit();
		}
}